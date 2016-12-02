#include "Center.h"
#include "SQLiteUtil.h"
#include "Datetime.h"
#include "PathUtils.h"

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
	clientDir_ = "Clients";
	engineDir_ = "EngineVersions";
	contentDir_ = "Contents";
	extraDir_ = "Extras";

	if (!fs::exists(clientDir_)) {
		boost::system::error_code ec;
		if (!fs::create_directories(makeSafePath(clientDir_), ec)) {
			throw std::runtime_error("failed to create directory");
		}
	}

	if (!fs::exists(engineDir_)) {
		boost::system::error_code ec;
		if (!fs::create_directories(makeSafePath(engineDir_), ec)) {
			throw std::runtime_error("failed to create directory");
		}
	}

	if (!fs::exists(contentDir_)) {
		boost::system::error_code ec;
		if (!fs::create_directories(makeSafePath(contentDir_), ec)) {
			throw std::runtime_error("failed to create directory");
		}
	}

	if (!fs::exists(extraDir_)) {
		boost::system::error_code ec;
		if (!fs::create_directories(makeSafePath(extraDir_), ec)) {
			throw std::runtime_error("failed to create directory");
		}
	}

	db_.reset(new SQLite::Database("BaseServer.db", SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS Users ("
		"Username TEXT COLLATE NOCASE UNIQUE, \"Group\" TEXT, Info TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS EngineVersions ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE, "
		"Setup TEXT, UnSetup TEXT, "
		"UpTime DATETIME, Info TEXT, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Contents ("
		"Id TEXT, ParentId TEXT, "
		"Title TEXT, Page TEXT, Category TEXT, EngineName TEXT, "
		"EngineVersion TEXT, Startup TEXT, ImageCount INT, Desc TEXT, "
		"User TEXT, UpTime DATETIME, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Extras ("
		"Id TEXT, Title TEXT, Setup TEXT, "
		"User TEXT, UpTime DATETIME, Info TEXT, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Clients ("
		"Version TEXT COLLATE NOCASE, "
		"UpTime DATETIME, Info TEXT, State TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Info ("
		"Key TEXT UNIQUE, Value TEXT)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfUsers ON Users ("
		"\"Group\")");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfEngineVersions ON EngineVersions ("
		"Name, Version, UpTime)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfContents ON Contents ("
		"Id, ParentId, Page, Category, EngineName, EngineVersion, User, UpTime, State)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfExtras ON Extras ("
		"Id, Title, User, UpTime, State)");

	db_->exec("CREATE INDEX IF NOT EXISTS IndexOfClients ON Clients ("
		"Version, UpTime, State)");

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
		s += category;
		s += "\n";
	}

	std::ostringstream oss;
	oss << "INSERT OR REPLACE INTO Info VALUES ('Categories', ";
	oss << sqlText(s);
	oss << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	categories_ = categories;

	updateCategoryGroup();
}

void Center::getCategories(std::vector<std::string>& categories)
{
	boost::mutex::scoped_lock lock(categoriesSync_);
	categories = categories_;
}

void Center::getGroupedCategories(std::map<std::string, std::string>& groupedCategories)
{
	boost::mutex::scoped_lock lock(categoriesSync_);
	groupedCategories = groupedCategories_;
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

bool Center::lockClientVersion(const std::string& version, LockMode mode)
{
	const std::string& key = boost::to_lower_copy(version);
	boost::mutex::scoped_lock lock(lockedClientVersionSetSync_);
	int& n = lockedClientVersionSet_[key];
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

void Center::unlockClientVersion(const std::string& version, LockMode mode)
{
	const std::string& key = boost::to_lower_copy(version);
	boost::mutex::scoped_lock lock(lockedClientVersionSetSync_);
	assert(lockedClientVersionSet_.count(key) == 1);
	int& n = lockedClientVersionSet_[key];
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

std::string Center::getContentPath(const std::string& id)
{
	std::vector<std::string> parts;
	boost::split(parts, id, boost::is_any_of("-"));

	fs::path path = contentDir();
	for (const std::string& part : parts) {
		path /= part;
	}

	return path.string();
}

std::string Center::getExtraPath(const std::string& id)
{
	std::vector<std::string> parts;
	boost::split(parts, id, boost::is_any_of("-"));

	fs::path path = extraDir();
	for (const std::string& part : parts) {
		path /= part;
	}

	return path.string();
}

std::string Center::getClientPath(const std::string& version)
{
	fs::path path = fs::path(clientDir()) / ("client_" + version);
	return path.string();
}

bool Center::addContent(const Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "INSERT INTO Contents VALUES (";
	oss << sqlText(id) << ", ";
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
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::updateContent(const Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "UPDATE Contents SET ";
	if (form.count("ParentId")) {
		oss << "ParentId=" << sqlText(form.at("ParentId")) << ", ";
	}
	oss << "Title=" << sqlText(form.at("Title")) << ", ";
	oss << "Page=" << sqlText(form.at("Page")) << ", ";
	oss << "Category=" << sqlText(form.at("Category")) << ", ";
	oss << "EngineName=" << sqlText(form.at("EngineName")) << ", ";
	oss << "EngineVersion=" << sqlText(form.at("EngineVersion")) << ", ";
	oss << "Startup=" << sqlText(form.at("Startup")) << ", ";
	oss << "Desc=" << sqlText(form.at("Desc"));
	oss << " WHERE Id=" << sqlText(id);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::getContent(Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Contents";
	oss << " WHERE ";
	oss << "Id=" << sqlText(id);

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

bool Center::changeContentState(const std::string& id, const std::string& state)
{
	std::ostringstream oss;
	oss << "UPDATE Contents SET State=";
	oss << sqlText(state);
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::addEngineVersion(const std::string& name, const std::string& version, const Form& form)
{
	std::ostringstream oss;
	oss << "INSERT INTO EngineVersions VALUES (";
	oss << sqlText(name) << ", ";
	oss << sqlText(version) << ", ";
	oss << sqlText(form.at("Setup")) << ", ";
	oss << sqlText(form.at("UnSetup")) << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText(form.at("Info")) << ", ";
	oss << sqlText("Normal") << ")";

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::updateEngineVersion(const std::string& name, const std::string& version, const Form& form)
{
	std::ostringstream oss;
	oss << "UPDATE EngineVersions SET ";
	oss << "Setup=" << sqlText(form.at("Setup")) << ", ";
	oss << "UnSetup=" << sqlText(form.at("UnSetup")) << ", ";
	oss << "Info=" << sqlText(form.at("Info"));
	oss << " WHERE ";
	oss << "Name=" << sqlText(name);
	oss << " AND ";
	oss << "Version=" << sqlText(version);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::getEngineVersion(const std::string& name, const std::string& version, Form& form)
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

	form["Setup"] = s.getColumn("Setup").getText();
	form["UnSetup"] = s.getColumn("UnSetup").getText();
	form["UpTime"] = s.getColumn("UpTime").getText();
	form["Info"] = s.getColumn("Info").getText();
	form["State"] = s.getColumn("State").getText();

	return true;
}

bool Center::getEngineVersionState(const std::string& name, const std::string& version, std::string& outState)
{
	std::ostringstream oss;
	oss << "SELECT State FROM EngineVersions";
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

bool Center::changeEngineVersionState(const std::string& name, const std::string& version, const std::string& state)
{
	std::ostringstream oss;
	oss << "UPDATE EngineVersions SET State=";
	oss << sqlText(state);
	oss << " WHERE Name=";
	oss << sqlText(name);
	oss << " AND Version=";
	oss << sqlText(version);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::addExtra(const Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "INSERT INTO Extras VALUES (";
	oss << sqlText(id) << ", ";
	oss << sqlText(form.at("Title")) << ", ";
	oss << sqlText(form.at("Setup")) << ", ";
	oss << sqlText(form.at("User")) << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText(form.at("Info")) << ", ";
	oss << sqlText("Normal") << ")";

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::updateExtra(const Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "UPDATE Extras SET ";
	oss << "Title=" << sqlText(form.at("Title")) << ", ";
	oss << "Setup=" << sqlText(form.at("Setup")) << ", ";
	oss << "Info=" << sqlText(form.at("Info"));
	oss << " WHERE Id=" << sqlText(id);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::getExtra(Form& form, const std::string& id)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Extras";
	oss << " WHERE ";
	oss << "Id=" << sqlText(id);

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return false;
	}

	form["Id"] = s.getColumn("Id").getText();
	form["Title"] = s.getColumn("Title").getText();
	form["Setup"] = s.getColumn("Setup").getText();
	form["User"] = s.getColumn("User").getText();
	form["UpTime"] = s.getColumn("UpTime").getText();
	form["Info"] = s.getColumn("Info").getText();
	form["State"] = s.getColumn("State").getText();

	return true;
}

bool Center::changeExtraState(const std::string& id, const std::string& state)
{
	std::ostringstream oss;
	oss << "UPDATE Extras SET State=";
	oss << sqlText(state);
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

std::string Center::getNewestClientVersion()
{
	std::ostringstream oss;
	oss << "SELECT * FROM Clients";
	oss << " WHERE State=" << sqlText("Normal");
	oss << " ORDER BY UpTime DESC";

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return "";
	}

	return s.getColumn("Version").getText();
}

bool Center::addClientVersion(const std::string& version, const Form& form)
{
	std::ostringstream oss;
	oss << "INSERT INTO Clients VALUES (";
	oss << sqlText(version) << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText(form.at("Info")) << ", ";
	oss << sqlText("Normal") << ")";

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::updateClientVersion(const std::string& version, const Form& form)
{
	std::ostringstream oss;
	oss << "UPDATE Clients SET ";
	oss << "Info=" << sqlText(form.at("Info"));
	oss << " WHERE ";
	oss << "Version=" << sqlText(version);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
}

bool Center::getClientVersion(const std::string& version, Form& form)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Clients";
	oss << " WHERE ";
	oss << "Version=" << sqlText(version);

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return false;
	}

	form["UpTime"] = s.getColumn("UpTime").getText();
	form["Info"] = s.getColumn("Info").getText();
	form["State"] = s.getColumn("State").getText();

	return true;
}

bool Center::getClientVersionState(const std::string& version, std::string& outState)
{
	std::ostringstream oss;
	oss << "SELECT State FROM Clients";
	oss << " WHERE ";
	oss << "Version=" << sqlText(version);

	SQLite::Statement s(*db_, oss.str());
	if (!s.executeStep()) {
		return false;
	}

	outState = s.getColumn("State").getText();

	return true;
}

bool Center::changeClientVersionState(const std::string& version, const std::string& state)
{
	std::ostringstream oss;
	oss << "UPDATE Clients SET State=";
	oss << sqlText(state);
	oss << " WHERE Version=";
	oss << sqlText(version);

	SQLite::Transaction t(*db_);
	int n = db_->exec(oss.str());
	t.commit();

	return (n > 0);
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

	std::istringstream stream(col.getText());
	std::string line;
	while (std::getline(stream, line)) {
		categories_.push_back(line);
	}

	updateCategoryGroup();
}

void Center::updateCategoryGroup()
{
	groupedCategories_.clear();

	std::string group;

	for (std::string s : categories_)
	{
		boost::trim(s);

		if (s.empty()) {
			continue;
		}

		if (boost::starts_with(s, "=")) {
			group = s;
		}
		else if (!group.empty()) {
			groupedCategories_.insert(std::make_pair(s, group));
		}
	}
}

