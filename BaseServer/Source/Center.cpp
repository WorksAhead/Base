#include "Center.h"
#include "SQLiteUtil.h"
#include "Datetime.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <assert.h>

namespace fs = boost::filesystem;

Center::Center()
{
	const fs::path currentPath = boost::filesystem::current_path();

	baseDir_ = currentPath.string();
	engineDir_ = (currentPath / "EngineVersions").string();

	if (!fs::exists(engineDir_) && !fs::create_directories(engineDir_)) {
		throw std::runtime_error("failed to create directory");
	}

	db_.reset(new SQLite::Database((currentPath / "BaseServer.db").string(), SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS Users ("
		"Username TEXT COLLATE NOCASE UNIQUE, Password TEXT, \"Group\" TEXT, "
		"RegTime DATETIME, Info TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS EngineVersions ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE, "
		"UpTime DATETIME, Info TEXT, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Info ("
		"Key TEXT UNIQUE, Value TEXT)");

	loadPagesFromDb();
	loadCategoriesFromDb();
}

Center::~Center()
{
}

void Center::setPages(const std::vector<std::string>& pages)
{
	boost::mutex::scoped_lock lock(pagesSync_);

	std::string s;
	for (const std::string& page : pages) {
		if (s.size()) {
			s += ",";
		}
		s += page;
	}

	std::ostringstream oss;
	oss << "INSERT OR REPLACE INTO Info VALUES ('Pages', ";
	oss << sqlText(s);
	oss << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	pages_ = pages;
}

void Center::getPages(std::vector<std::string>& pages)
{
	boost::mutex::scoped_lock lock(pagesSync_);
	pages = pages_;
}

void Center::setCategories(const std::vector<std::string>& categories)
{
	boost::mutex::scoped_lock lock(categoriesSync_);

	std::string s;
	for (const std::string& category : categories) {
		if (s.size()) {
			s += ",";
		}
		s += category;
	}

	std::ostringstream oss;
	oss << "INSERT OR REPLACE INTO Info VALUES ('Categories', ";
	oss << sqlText(s);
	oss << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	categories_ = categories;
}

void Center::getCategories(std::vector<std::string>& categories)
{
	boost::mutex::scoped_lock lock(categoriesSync_);
	categories = categories_;
}

bool Center::lockEngineVersion(const std::string& name, const std::string& version, LockMode mode)
{
	boost::mutex::scoped_lock lock(lockedEngineVersionSetSync_);
	int& n = lockedEngineVersionSet_[name + "\n" + version];
	if (mode == lock_write && n == 0) {
		n = -1;
		return true;
	}
	else if (mode == lock_read && n >= 0) {
		++n;
		return true;
	}
	else {
		return false;
	}
}

void Center::unlockEngineVersion(const std::string& name, const std::string& version, LockMode mode)
{
	boost::mutex::scoped_lock lock(lockedEngineVersionSetSync_);
	assert(lockedEngineVersionSet_.count(name + "\n" + version) == 1);
	int& n = lockedEngineVersionSet_[name + "\n" + version];
	if (mode == lock_write) {
		assert(n == -1);
		n = 0;
	}
	else if (mode == lock_read) {
		assert(n > 0);
		--n;
	}
}

std::string Center::engineFileName(const std::string& name, const std::string& version) const
{
	fs::path path = fs::path(engineDir()) /
		boost::to_lower_copy(
		boost::replace_all_copy(name, " ", "_") +
		"-" +
		boost::replace_all_copy(version, " ", "_")
		);

	return path.string();
}

bool Center::getEngineVersionState(const std::string& name, const std::string& version, std::string& outState)
{
	std::ostringstream oss;
	oss << "SELECT * FROM EngineVersions";
	oss << " WHERE ";
	oss << "Name=" << sqlText(name);
	oss << " AND ";
	oss << "Version=" << sqlText(version);

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return false;
	}

	SQLite::Column col = s.getColumn("State");
	outState = col.getText();

	return true;
}

void Center::addEngineVersion(const std::string& name, const std::string& version, const std::string& info)
{
	std::ostringstream oss;
	oss << "INSERT INTO EngineVersions VALUES (";
	oss << sqlText(name) << ", ";
	oss << sqlText(version) << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText(info) << ", ";
	oss << sqlText("Normal") << ")";

	std::ostringstream oss2;
	oss2 << "INSERT OR IGNORE INTO EngineNames VALUES (";
	oss2 << sqlText(name) << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	db_->exec(oss2.str());
	t.commit();
}

void Center::changeEngineVersionState(const std::string& name, const std::string& version, const std::string& state)
{
	std::ostringstream oss;
	oss << "UPDATE EngineVersions SET State=";
	oss << sqlText(state);
	oss << " WHERE Name=";
	oss << sqlText(name);
	oss << " AND Version=";
	oss << sqlText(version);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();
}

void Center::loadPagesFromDb()
{
	boost::mutex::scoped_lock lock(pagesSync_);

	SQLite::Statement s(*db_, "SELECT Value FROM Info WHERE Key='Pages'");
	if (!s.executeStep()) {
		return;
	}

	SQLite::Column col = s.getColumn("Value");

	boost::split(pages_, std::string(col.getText()), boost::is_any_of(","), boost::token_compress_on);
}

void Center::loadCategoriesFromDb()
{
	boost::mutex::scoped_lock lock(categoriesSync_);

	SQLite::Statement s(*db_, "SELECT Value FROM Info WHERE Key='Categories'");
	if (!s.executeStep()) {
		return;
	}

	SQLite::Column col = s.getColumn("Value");

	boost::split(categories_, std::string(col.getText()), boost::is_any_of(","), boost::token_compress_on);
}

