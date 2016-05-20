#include "Center.h"
#include "SQLiteUtil.h"
#include "Datetime.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <boost/filesystem.hpp>

#include <sstream>
#include <assert.h>

namespace fs = boost::filesystem;

Center::Center()
{
	const fs::path currentPath = boost::filesystem::current_path();

	baseDir_ = currentPath.string();
	engineDir_ = (currentPath / "Engines").string();

	if (!fs::exists(engineDir_) && !fs::create_directories(engineDir_)) {
		throw std::runtime_error("failed to create directory");
	}

	db_.reset(new SQLite::Database((currentPath / "BaseServer.db").string(), SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS Users ("
		"Username TEXT COLLATE NOCASE UNIQUE, Password TEXT, \"Group\" TEXT, "
		"RegTime DATETIME, Info TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Engines ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE, "
		"UpTime DATETIME, Info TEXT, State TEXT)");
}

Center::~Center()
{
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
	oss << "SELECT * FROM Engines";
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
	oss << "INSERT INTO Engines VALUES (";
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
	oss << "UPDATE Engines SET State=";
	oss << sqlText(state);
	oss << " WHERE Name=";
	oss << sqlText(name);
	oss << " AND Version=";
	oss << sqlText(version);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();
}

