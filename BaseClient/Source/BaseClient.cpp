#include "BaseClient.h"
#include "PageWidget.h"
#include "LibraryWidget.h"
#include "ManageWidget.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncCreateProjectTask.h"
#include "ASyncRemoveTask.h"
#include "ContentImageLoader.h"
#include "ErrorMessage.h"
#include "QtUtils.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <QBoxLayout>
#include <QTabBar>
#include <QTextEdit>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>

namespace fs = boost::filesystem;

class RefreshTask : public IceUtil::TimerTask {
public:
	RefreshTask(Rpc::SessionPrx session) : session_(session)
	{
	}

	virtual void runTimerTask()
	{
		try {
			session_->refresh();
		}
		catch (const Ice::Exception& e) {
		}
	}

private:
	Rpc::SessionPrx session_;
};

BaseClient::BaseClient(Rpc::SessionPrx session)
{
	setWindowIcon(QIcon(":/Icons/Base20x20.png"));
	setWindowTitle("Base");

	timer_ = new IceUtil::Timer;
	timer_->scheduleRepeated(new RefreshTask(session), IceUtil::Time::seconds(5));

	if (!fs::exists("Cache") && !fs::create_directories("Cache")) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists("Library/Contents") && !fs::create_directories("Library/Contents")) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists("Library/Engines") && !fs::create_directories("Library/Engines")) {
		throw std::runtime_error("Failed to create directory");
	}

	initDb();
	loadDownloadedContentsFromDb();
	loadInstalledEnginesFromDb();
	loadProjectsFromDb();

	context_.reset(new Context);
	context_->session = session;
	context_->contentImageLoader = new ContentImageLoader(context_, this);
	context_->addTask = std::bind(&BaseClient::addTask, this, std::placeholders::_1);
	context_->showTaskManager = std::bind(&BaseClient::onShowTaskManager, this);
	context_->uniquePath = std::bind(&BaseClient::uniquePath, this);
	context_->cachePath = std::bind(&BaseClient::cachePath, this);
	context_->libraryPath = std::bind(&BaseClient::libraryPath, this);
	context_->enginePath = std::bind(&BaseClient::enginePath, this, std::placeholders::_1, std::placeholders::_2);
	context_->contentPath = std::bind(&BaseClient::contentPath, this, std::placeholders::_1);
	context_->installEngine = std::bind(&BaseClient::installEngine, this, std::placeholders::_1, std::placeholders::_2);
	context_->getEngineState = std::bind(&BaseClient::getEngineState, this, std::placeholders::_1, std::placeholders::_2);
	context_->changeEngineState = std::bind(&BaseClient::changeEngineState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	context_->getDownloadedContentList = std::bind(&BaseClient::getDownloadedContentList, this, std::placeholders::_1);
	context_->getContentState = std::bind(&BaseClient::getContentState, this, std::placeholders::_1);
	context_->changeContentState = std::bind(&BaseClient::changeContentState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	context_->createProject = std::bind(&BaseClient::createProject, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	context_->addProject = std::bind(&BaseClient::addProject, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	context_->removeProject = std::bind(&BaseClient::removeProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->renameProject = std::bind(&BaseClient::renameProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->getProject = std::bind(&BaseClient::getProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->getProjectList = std::bind(&BaseClient::getProjectList, this, std::placeholders::_1);
	context_->promptRpcError = std::bind(&BaseClient::promptRpcError, this, std::placeholders::_1);

	taskManagerDialog_ = new ASyncTaskManagerDialog;

	QWidget* decoratorWidget = new QWidget;
	decoratorWidgetUi_.setupUi(decoratorWidget);

	setDecoratorWidget(decoratorWidget);

	lowerPane_ = new LowerPaneWidget(context_);

	tabWidget_ = new QTabWidget;
	tabWidget_->setAutoFillBackground(true);
	tabWidget_->setObjectName("MainTab");
	tabWidget_->tabBar()->setObjectName("MainTabBar");

	QFont font = tabWidget_->tabBar()->font();
	font.setPixelSize(16);
	tabWidget_->tabBar()->setFont(font);

	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = session->getPages(pages);
	if (ec != Rpc::ec_success) {
		promptRpcError(ec);
		throw int(0);
	}

	for (const std::string& page : pages) {
		tabWidget_->addTab(new PageWidget(context_, page.c_str()), page.c_str());
	}

	library_ = new LibraryWidget(context_);
	tabWidget_->addTab(library_, "Library");

	manage_ = new ManageWidget(context_);
	tabWidget_->addTab(manage_, "Manage");

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(tabWidget_);
	layout->addWidget(lowerPane_);
	QWidget* w = new QWidget;
	w->setLayout(layout);
	setCentralWidget(w);

	QObject::connect(decoratorWidgetUi_.taskButton, &QPushButton::clicked, this, &BaseClient::onShowTaskManager);
	QObject::connect(taskManagerDialog_, &ASyncTaskManagerDialog::cleared, lowerPane_, &LowerPaneWidget::clear);
}

BaseClient::~BaseClient()
{
}

void BaseClient::addTask(ASyncTaskPtr task)
{
	task->start();

	lowerPane_->addTask(task);
	taskManagerDialog_->listWidget()->addTask(task);
}

std::string BaseClient::uniquePath()
{
	fs::path p = fs::temp_directory_path();
	p = p / boost::uuids::to_string(rand_());
	return p.string();
}

std::string BaseClient::cachePath()
{
	return "Cache";
}

std::string BaseClient::libraryPath()
{
	return "Library";
}

std::string BaseClient::enginePath(const std::string& name, const std::string& version)
{
	fs::path path = libraryPath();
	path /= "Engines";
	path /= boost::to_lower_copy(
		boost::replace_all_copy(name, " ", "_") +
		"-" +
		boost::replace_all_copy(version, " ", "_")
		);
	return path.string();
}

std::string BaseClient::contentPath(const std::string& id)
{
	fs::path path = libraryPath();
	path /= "Contents";
	path /= id;
	return path.string();
}

void BaseClient::installEngine(const std::string& name, const std::string& version)
{
	int state = EngineState::not_installed;

	if (!context_->changeEngineState(name, version, state, EngineState::installing))
	{
		if (state == EngineState::installing) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is now installing").arg(name.c_str()).arg(version.c_str()));
		}
		else if (state == EngineState::installed) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is already installed").arg(name.c_str()).arg(version.c_str()));
		}
		else if (state == EngineState::removing) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is now removing").arg(name.c_str()).arg(version.c_str()));
		}
		return;
	}

	Rpc::DownloaderPrx downloader;
	Rpc::ErrorCode ec = context_->session->downloadEngineVersion(name, version, downloader);
	if (ec != Rpc::ec_success) {
		state = EngineState::installing;
		context_->changeEngineState(name, version, state, EngineState::not_installed);
		QMessageBox::information(this, "Base", QString("Unable to download %1 %2").arg(name.c_str()).arg(version.c_str()));
		return;
	}

	boost::shared_ptr<ASyncInstallEngineTask> task(new ASyncInstallEngineTask(context_, downloader));
	task->setInfoHead("Install " + name + " " + version);
	task->setEngineVersion(name, version);
	task->setPath(context_->enginePath(name, version));

	addTask(task);
}

int BaseClient::getEngineState(const std::string& name, const std::string& version)
{
	const std::string& key = boost::to_lower_copy(name + "\n" + version);

	boost::recursive_mutex::scoped_lock lock(engineStateTabelSync_);

	if (engineStateTabel_.count(key) == 0) {
		engineStateTabel_.insert(std::make_pair(key, EngineState::not_installed));
	}

	return engineStateTabel_[key];
}

bool BaseClient::changeEngineState(const std::string& name, const std::string& version, int& oldState, int newState)
{
	const std::string& key = boost::to_lower_copy(name + "\n" + version);

	boost::recursive_mutex::scoped_lock lock(engineStateTabelSync_);

	const int state = getEngineState(name, version);
	if (state != oldState) {
		oldState = state;
		return false;
	}

	if (newState == oldState) {
		return true;
	}

	if (newState == EngineState::installed)
	{
		std::ostringstream oss;
		oss << "INSERT INTO InstalledEngines VALUES (";
		oss << sqlText(name) << ", ";
		oss << sqlText(version) << ")";

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);
		installedEngineTabel_.insert(key);
	}
	else if (newState == EngineState::removing)
	{
		std::ostringstream oss;
		oss << "DELETE FROM InstalledEngines";
		oss << " WHERE Name=";
		oss << sqlText(name);
		oss << " AND Version=";
		oss << sqlText(version);

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);
		installedEngineTabel_.erase(key);
	}

	engineStateTabel_[key] = newState;

	return true;
}

void BaseClient::getDownloadedContentList(std::vector<std::string>& outList)
{
	outList.clear();
	boost::recursive_mutex::scoped_lock lock(downloadedContentTabelSync_);
	for (const std::string& id : downloadedContentTabel_) {
		outList.push_back(id);
	}
}

int BaseClient::getContentState(const std::string& id)
{
	boost::recursive_mutex::scoped_lock lock(contentStateTabelSync_);

	if (contentStateTabel_.count(id) == 0) {
		contentStateTabel_.insert(std::make_pair(id, ContentState::not_downloaded));
	}

	return contentStateTabel_[id];
}

bool BaseClient::changeContentState(const std::string& id, int& oldState, int newState)
{
	boost::recursive_mutex::scoped_lock lock(engineStateTabelSync_);

	const int state = getContentState(id);
	if (state != oldState) {
		oldState = state;
		return false;
	}

	if (newState == oldState) {
		return true;
	}

	if (newState == ContentState::downloaded)
	{
		std::ostringstream oss;
		oss << "INSERT INTO DownloadedContents VALUES (";
		oss << sqlText(id) << ")";

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		QMetaObject::invokeMethod(library_, "addContent", Qt::QueuedConnection, Q_ARG(QString, id.c_str()));

		boost::recursive_mutex::scoped_lock lock(downloadedContentTabelSync_);
		downloadedContentTabel_.insert(id);
	}
	else if (newState == ContentState::not_downloaded)
	{
		std::ostringstream oss;
		oss << "DELETE FROM DownloadedContents";
		oss << " WHERE Id=";
		oss << sqlText(id);

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		QMetaObject::invokeMethod(library_, "removeContent", Qt::QueuedConnection, Q_ARG(QString, id.c_str()));

		boost::recursive_mutex::scoped_lock lock(downloadedContentTabelSync_);
		downloadedContentTabel_.erase(id);
	}

	contentStateTabel_[id] = newState;

	return true;
}

void BaseClient::createProject(const std::string& id, const std::string& title, const std::string& location)
{
	boost::shared_ptr<ASyncCreateProjectTask> task(new ASyncCreateProjectTask(context_));
	task->setInfoHead("Create " + title);
	task->setContentId(id);
	task->setProjectId(boost::uuids::to_string(rand_()));
	task->setProjectName(title);
	task->setLocation(location);

	addTask(task);
}

void BaseClient::addProject(const std::string& id, const std::string& contentId, const std::string& location, const std::string& name)
{
	std::ostringstream oss;
	oss << "INSERT INTO Projects VALUES (";
	oss << sqlText(id) << ", ";
	oss << sqlText(contentId) << ", ";
	oss << sqlText(fromLocal8bit(location)) << ", ";
	oss << sqlText(fromLocal8bit(name)) << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	QMetaObject::invokeMethod(library_, "addProject", Qt::QueuedConnection, Q_ARG(QString, id.c_str()));

	ProjectInfo pi;
	pi.id = id;
	pi.contentId = contentId;
	pi.location = location;
	pi.name = name;

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	projectTabel_.insert(std::make_pair(id, pi));
}

void BaseClient::removeProject(const std::string& id, bool removeDir)
{
	std::ostringstream oss;
	oss << "DELETE FROM Projects";
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	QMetaObject::invokeMethod(library_, "removeProject", Qt::QueuedConnection, Q_ARG(QString, id.c_str()));

	if (removeDir)
	{
		ProjectInfo pi;
		if (getProject(pi, id))
		{
			boost::shared_ptr<ASyncRemoveTask> task(new ASyncRemoveTask);
			task->setInfoHead("Remove " + pi.name);
			task->setPath(pi.location);
			addTask(task);
		}
	}

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	projectTabel_.erase(id.c_str());
}

void BaseClient::renameProject(const std::string& id, const std::string& newName)
{
	std::ostringstream oss;
	oss << "UPDATE Projects SET Name=";
	oss << sqlText(fromLocal8bit(newName));
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	auto it = projectTabel_.find(id);
	if (it != projectTabel_.end()) {
		it->second.name = newName;
	}
}

bool BaseClient::getProject(ProjectInfo& outInfo, const std::string& id)
{
	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	auto it = projectTabel_.find(id);
	if (it != projectTabel_.end()) {
		outInfo = it->second;
		return true;
	}

	return false;
}

void BaseClient::getProjectList(std::vector<ProjectInfo>& outList)
{
	std::vector<ProjectInfo> list;
	{
		boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
		for (auto& p : projectTabel_) {
			list.push_back(p.second);
		}
	}

	std::sort(list.begin(), list.end(), [](const ProjectInfo& lhs, const ProjectInfo& rhs){
		return (lhs.name < rhs.name);
	});

	outList = std::move(list);
}

void BaseClient::promptRpcError(Rpc::ErrorCode ec)
{
	QMessageBox::critical(this, "Base", errorMessage(ec));
	return;
}

void BaseClient::onShowTaskManager()
{
	if (taskManagerDialog_->isHidden()) {
		taskManagerDialog_->show();
	}
	else if (!taskManagerDialog_->isActiveWindow()) {
		QApplication::setActiveWindow(taskManagerDialog_);
	}
}

void BaseClient::initDb()
{
	db_.reset(new SQLite::Database("BaseClient.db", SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS InstalledEngines ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE)");

	db_->exec("CREATE TABLE IF NOT EXISTS DownloadedContents ("
		"Id TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Projects ("
		"Id TEXT, ContentId TEXT, Location TEXT, Name TEXT)");
}

void BaseClient::loadDownloadedContentsFromDb()
{
	std::ostringstream oss;
	oss << "SELECT * FROM DownloadedContents";

	boost::recursive_mutex::scoped_lock lock(downloadedContentTabelSync_);

	SQLite::Statement s(*db_, oss.str());
	while (s.executeStep()) {
		downloadedContentTabel_.insert(s.getColumn("Id").getText());
	}

	boost::recursive_mutex::scoped_lock lock2(contentStateTabelSync_);

	for (const std::string& id : downloadedContentTabel_) {
		contentStateTabel_[id] = ContentState::downloaded;
	}
}

void BaseClient::loadInstalledEnginesFromDb()
{
	std::ostringstream oss;
	oss << "SELECT * FROM InstalledEngines";

	boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);

	SQLite::Statement s(*db_, oss.str());
	while (s.executeStep()) {
		const std::string& name = s.getColumn("Name").getText();
		const std::string& version = s.getColumn("Version").getText();
		const std::string& key = boost::to_lower_copy(name + "\n" + version);
		installedEngineTabel_.insert(key);
	}

	boost::recursive_mutex::scoped_lock lock2(engineStateTabelSync_);

	for (const std::string& key : installedEngineTabel_) {
		engineStateTabel_[key] = EngineState::installed;
	}
}

void BaseClient::loadProjectsFromDb()
{
	std::ostringstream oss;
	oss << "SELECT * FROM Projects";

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);

	SQLite::Statement s(*db_, oss.str());
	while (s.executeStep()) {
		ProjectInfo pi;
		pi.id = s.getColumn("Id").getText();
		pi.contentId = s.getColumn("ContentId").getText();
		pi.location = toLocal8bit(s.getColumn("Location").getText());
		pi.name = toLocal8bit(s.getColumn("Name").getText());
		projectTabel_.insert(std::make_pair(pi.id, pi));
	}
}

