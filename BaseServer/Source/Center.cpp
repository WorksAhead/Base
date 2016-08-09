#include "Center.h"
#include "SQLiteUtil.h"
#include "Datetime.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <assert.h>

namespace fs = boost::filesystem;

Center::Center()
{
	engineDir_ = "EngineVersions";
	contentDir_ = "Contents";

	if (!fs::exists(engineDir_) && !fs::create_directories(engineDir_)) {
		throw std::runtime_error("failed to create directory");
	}

	if (!fs::exists(contentDir_) && !fs::create_directories(contentDir_)) {
		throw std::runtime_error("failed to create directory");
	}

	db_.reset(new SQLite::Database("BaseServer.db", SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS Users ("
		"Username TEXT COLLATE NOCASE UNIQUE, Password TEXT, \"Group\" TEXT, "
		"RegTime DATETIME, Info TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS EngineVersions ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE, "
		"UpTime DATETIME, Info TEXT, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Contents ("
		"Id TEXT, ParentId TEXT, "
		"Title TEXT, Page TEXT, Category TEXT, EngineName TEXT, "
		"EngineVersion TEXT, Startup TEXT, ImageCount INT, Desc TEXT, "
		"User TEXT, UpTime DATETIME, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Info ("
		"Key TEXT UNIQUE, Value TEXT)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfUsers ON Users ("
		"\"Group\")");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfEngineVersions ON EngineVersions ("
		"Name, Version, UpTime)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfContents ON Contents ("
		"Id, ParentId, Page, Category, EngineName, EngineVersion, User, UpTime, State)");

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
	const std::string& key = boost::to_lower_copy(name + "\n" + version);
	boost::mutex::scoped_lock lock(lockedEngineVersionSetSync_);
	int& n = lockedEngineVersionSet_[key];
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
	const std::string& key = boost::to_lower_copy(name + "\n" + version);
	boost::mutex::scoped_lock lock(lockedEngineVersionSetSync_);
	assert(lockedEngineVersionSet_.count(key) == 1);
	int& n = lockedEngineVersionSet_[key];
	if (mode == lock_write) {
		assert(n == -1);
		n = 0;
	}
	else if (mode == lock_read) {
		assert(n > 0);
		--n;
	}
}

std::string Center::generateUuid()
{
	return boost::uuids::to_string(uniquePathGen_());
}

std::string Center::getEnginePath(const std::string& name, const std::string& version)
{
	fs::path path = fs::path(engineDir()) /
		boost::to_lower_copy(
		boost::replace_all_copy(name, " ", "_") +
		"-" +
		boost::replace_all_copy(version, " ", "_")
		);

	return path.string();
}

std::string Center::getContentPath(const std::string& uid)
{
	std::vector<std::string> parts;
	boost::split(parts, uid, boost::is_any_of("-"));

	fs::path path = contentDir();
	for (const std::string& part : parts) {
		path /= part;
	}

	return path.string();
}

void Center::addContent(const std::map<std::string, std::string>& form, const std::string& uid)
{
	std::ostringstream oss;
	oss << "INSERT INTO Contents VALUES (";
	oss << sqlText(uid) << ", ";
	oss << sqlText(form.at("ParentId")) << ", ";
	oss << sqlText(form.at("Title")) << ", ";
	oss << sqlText(form.at("Page")) << ", ";
	oss << sqlText(form.at("Category")) << ", ";
	oss << sqlText(form.at("EngineName")) << ", ";
	oss << sqlText(form.at("EngineVersion")) << ", ";
	oss << sqlText(form.at("Startup")) << ", ";
	oss << sqlText(form.at("ImageCount")) << ", ";
	oss << sqlText(form.at("Desc")) << ", ";
	oss << sqlText(form.at("User")) << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText("Normal") << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();
}

bool Center::getContent(std::map<std::string, std::string>& form, const std::string& uid)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Contents";
	oss << " WHERE ";
	oss << "Id=" << sqlText(uid);

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return false;
	}
	
	form["Id"] = s.getColumn("Id").getText();
	form["ParentId"] = s.getColumn("ParentId").getText();
	form["Title"] = s.getColumn("Title").getText();
	form["Page"] = s.getColumn("Page").getText();
	form["Category"] = s.getColumn("Category").getText();
	form["EngineName"] = s.getColumn("EngineName").getText();
	form["EngineVersion"] = s.getColumn("EngineVersion").getText();
	form["Startup"] = s.getColumn("Startup").getText();
	form["ImageCount"] = s.getColumn("ImageCount").getText();
	form["Desc"] = s.getColumn("Desc").getText();
	form["User"] = s.getColumn("User").getText();
	form["UpTime"] = s.getColumn("UpTime").getText();
	form["State"] = s.getColumn("State").getText();

	return true;
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

	outState = s.getColumn("State").getText();

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

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
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

bool Center::setUserGroup(const std::string& username, const std::string& group)
{
	std::ostringstream oss;
	oss << "UPDATE Users SET \"Group\"=";
	oss << sqlText(group);
	oss << " WHERE Username=";
	oss << sqlText(username);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::removeUser(const std::string& username)
{
	std::ostringstream oss;
	oss << "DELETE FROM Users";
	oss << " WHERE Username=";
	oss << sqlText(username);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
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

