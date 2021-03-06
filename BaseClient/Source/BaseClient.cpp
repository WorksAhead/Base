#include "BaseClient.h"
#include "PageContentWidget.h"
#include "PageWebWidget.h"
#include "PageEngineWidget.h"
#include "PageExtraWidget.h"
#include "HTabWidget.h"
#include "LibraryWidget.h"
#include "ManageWidget.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncRemoveEngineTask.h"
#include "ASyncInstallExtraTask.h"
#include "ASyncCreateProjectTask.h"
#include "ASyncRemoveTask.h"
#include "ContentImageLoader.h"
#include "ExtraImageLoader.h"
#include "ErrorMessage.h"
#include "Datetime.h"
#include "QtUtils.h"
#include "Base64Url.h"
#include "URLUtils.h"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <QBoxLayout>
#include <QTabBar>
#include <QTextEdit>
#include <QThread>
#include <QProcess>
#include <QMessageBox>
#include <QLocalSocket>
#include <QSettings>
#include <QDesktopServices>
#include <QTimer>
#include <QCloseEvent>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <sstream>
#include <memory>

#if WIN32
#include <windows.h>
#endif

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

BaseClient::BaseClient(const QString& workPath, const QString& version, Rpc::SessionPrx session, const QString& url)
{
	workPath_ = toLocal8bit(workPath);

	setWindowIcon(QIcon(":/Icons/Base20x20.png"));
	setWindowTitle("Base " + version);

	timer_ = new IceUtil::Timer;
	timer_->scheduleRepeated(new RefreshTask(session), IceUtil::Time::seconds(5));

	localServer_ = new QLocalServer(this);
	if (!localServer_->listen("base_client_application_uri_server")) {
		prompt(1, toLocal8bit(localServer_->errorString()));
	}

	Qt::HANDLE guitid = QThread::currentThreadId();

	context_.reset(new Context);

	context_->session = session;

	std::string currentUser;

	if (session->getCurrentUser(currentUser) != Rpc::ec_success) {
		throw std::runtime_error("Failed to get current User");
	}

	std::string::size_type pos = currentUser.find('@');
	if (pos != std::string::npos) {
		currentUser.erase(pos);
	}

	context_->currentUser = currentUser;

	if (session->getCurrentUserGroup(context_->currentUserGroup) != Rpc::ec_success) {
		throw std::runtime_error("Failed to get current User Group");
	}

	context_->contentImageLoader = new ContentImageLoader(context_, this);
	context_->extraImageLoader = new ExtraImageLoader(context_, this);
	context_->getLastViewStamp = std::bind(&BaseClient::getLastViewStamp, this);
	context_->setLastViewStamp = std::bind(&BaseClient::setLastViewStamp, this, std::placeholders::_1);
	context_->addTask = std::bind(&BaseClient::addTask, this, std::placeholders::_1);
	context_->showTaskManager = std::bind(&BaseClient::onShowTaskManager, this);
	context_->uniquePath = std::bind(&BaseClient::uniquePath, this);
	context_->cachePath = std::bind(&BaseClient::cachePath, this);
	context_->enginePath = std::bind(&BaseClient::enginePath, this, std::placeholders::_1);
	context_->contentPath = std::bind(&BaseClient::contentPath, this, std::placeholders::_1);
	context_->extraPath = std::bind(&BaseClient::extraPath, this, std::placeholders::_1);
	context_->installEngine = std::bind(&BaseClient::installEngine, this, std::placeholders::_1);
	context_->setupEngine = std::bind(&BaseClient::setupEngine, this, std::placeholders::_1);
	context_->unSetupEngine = std::bind(&BaseClient::unSetupEngine, this, std::placeholders::_1);
	context_->removeEngine = std::bind(&BaseClient::removeEngine, this, std::placeholders::_1);
	context_->getEngineState = std::bind(&BaseClient::getEngineState, this, std::placeholders::_1);
	context_->changeEngineState = std::bind(&BaseClient::changeEngineState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	context_->getEngineList = std::bind(&BaseClient::getEngineList, this, std::placeholders::_1);

	context_->addEngineToGui = [guitid, this](const EngineVersion& v){
		if (QThread::currentThreadId() == guitid) {
			addEngineToGui(v);
		}
		else {
			QMetaObject::invokeMethod(this, "addEngineToGui", Qt::BlockingQueuedConnection, Q_ARG(EngineVersion, v));
		}
	};

	context_->removeEngineFromGui = [guitid, this](const EngineVersion& v){
		if (QThread::currentThreadId() == guitid) {
			removeEngineFromGui(v);
		}
		else {
			QMetaObject::invokeMethod(this, "removeEngineFromGui", Qt::BlockingQueuedConnection, Q_ARG(EngineVersion, v));
		}
	};

	context_->getDownloadedContentList = std::bind(&BaseClient::getDownloadedContentList, this, std::placeholders::_1);
	context_->getContentState = std::bind(&BaseClient::getContentState, this, std::placeholders::_1);
	context_->changeContentState = std::bind(&BaseClient::changeContentState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	context_->addContentToGui = [guitid, this](const std::string& contentId){
		if (QThread::currentThreadId() == guitid) {
			addContentToGui(contentId);
		}
		else {
			QMetaObject::invokeMethod(this, "addContentToGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, contentId));
		}
	};

	context_->removeContentFromGui = [guitid, this](const std::string& contentId){
		if (QThread::currentThreadId() == guitid) {
			removeContentFromGui(contentId);
		}
		else {
			QMetaObject::invokeMethod(this, "removeContentFromGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, contentId));
		}
	};

	context_->getDownloadedExtraList = std::bind(&BaseClient::getDownloadedExtraList, this, std::placeholders::_1);
	context_->installExtra = std::bind(&BaseClient::installExtra, this, std::placeholders::_1);
	context_->setupExtra = std::bind(&BaseClient::setupExtra, this, std::placeholders::_1);
	context_->getExtraState = std::bind(&BaseClient::getExtraState, this, std::placeholders::_1);
	context_->changeExtraState = std::bind(&BaseClient::changeExtraState, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	context_->addExtraToGui = [guitid, this](const std::string& id){
		if (QThread::currentThreadId() == guitid) {
			addExtraToGui(id);
		}
		else {
			QMetaObject::invokeMethod(this, "addExtraToGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, id));
		}
	};

	context_->removeExtraFromGui = [guitid, this](const std::string& id){
		if (QThread::currentThreadId() == guitid) {
			removeExtraFromGui(id);
		}
		else {
			QMetaObject::invokeMethod(this, "removeExtraFromGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, id));
		}
	};

	context_->createProject = std::bind(&BaseClient::createProject, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	context_->addProject = std::bind(&BaseClient::addProject, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	context_->removeProject = std::bind(&BaseClient::removeProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->renameProject = std::bind(&BaseClient::renameProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->changeProjectDefaultEngineVersion = std::bind(&BaseClient::changeProjectDefaultEngineVersion, this, std::placeholders::_1, std::placeholders::_2);
	context_->changeProjectStartup = std::bind(&BaseClient::changeProjectStartup, this, std::placeholders::_1, std::placeholders::_2);
	context_->getProject = std::bind(&BaseClient::getProject, this, std::placeholders::_1, std::placeholders::_2);
	context_->getProjectList = std::bind(&BaseClient::getProjectList, this, std::placeholders::_1);

	context_->addProjectToGui = [guitid, this](const std::string& projectId){
		if (QThread::currentThreadId() == guitid) {
			addProjectToGui(projectId);
		}
		else {
			QMetaObject::invokeMethod(this, "addProjectToGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, projectId));
		}
	};

	context_->removeProjectFromGui = [guitid, this](const std::string& projectId){
		if (QThread::currentThreadId() == guitid) {
			removeProjectFromGui(projectId);
		}
		else {
			QMetaObject::invokeMethod(this, "removeProjectFromGui", Qt::BlockingQueuedConnection, Q_ARG(std::string, projectId));
		}
	};

	context_->prompt = [guitid,this](int level, const std::string& message){
		if (QThread::currentThreadId() == guitid) {
			prompt(level, message);
		}
		else {
			QMetaObject::invokeMethod(this, "prompt", Qt::BlockingQueuedConnection, Q_ARG(int, level), Q_ARG(std::string, message));
		}
	};

	context_->promptRpcError = [guitid,this](Rpc::ErrorCode ec){
		if (QThread::currentThreadId() == guitid) {
			promptRpcError(ec);
		}
		else {
			QMetaObject::invokeMethod(this, "promptRpcError", Qt::BlockingQueuedConnection, Q_ARG(Rpc::ErrorCode, ec));
		}
	};

	context_->promptEngineState = [guitid,this](const EngineVersion& v, int state){
		if (QThread::currentThreadId() == guitid) {
			promptEngineState(v, state);
		}
		else {
			QMetaObject::invokeMethod(this, "promptEngineState", Qt::BlockingQueuedConnection, Q_ARG(EngineVersion, v), Q_ARG(int, state));
		}
	};

	context_->promptExtraState = [guitid, this](const std::string& title, int state){
		if (QThread::currentThreadId() == guitid) {
			promptExtraState(title, state);
		}
		else {
			QMetaObject::invokeMethod(this, "promptExtraState", Qt::BlockingQueuedConnection, Q_ARG(std::string, title), Q_ARG(int, state));
		}
	};

	context_->openUrl = [guitid, this](const std::string& url) {
		if (QThread::currentThreadId() == guitid) {
			openUrl(url.c_str());
		}
		else {
			QMetaObject::invokeMethod(this, "openUrl", Qt::BlockingQueuedConnection, Q_ARG(QString, url.c_str()));
		}
	};

	if (!fs::exists(cachePath()) && !fs::create_directories(cachePath())) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists(databasePath()) && !fs::create_directories(databasePath())) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists(outputPath()) && !fs::create_directories(outputPath())) {
		throw std::runtime_error("Failed to create directory");
	}

	initDb();
	loadDownloadedContentsFromDb();
	loadInstalledEnginesFromDb();
	loadDownloadedExtrasFromDb();
	loadProjectsFromDb();

	historyDialog_ = new HistoryDialog(context_, this);

	taskManagerDialog_ = new ASyncTaskManagerDialog(this);

	QWidget* expressWidget = new QWidget;
	expressWidgetUi_.setupUi(expressWidget);

	setExpressWidget(expressWidget);

	QWidget* decoratorWidget = new QWidget;
	decoratorWidgetUi_.setupUi(decoratorWidget);

	setDecoratorWidget(decoratorWidget);

	lowerPane_ = new LowerPaneWidget(context_);

	tabWidget_ = new HTabWidget;
	tabWidget_->setAutoFillBackground(true);
	tabWidget_->setObjectName("MainTab");
	tabWidget_->tabBar()->setObjectName("MainTabBar");

	QFont font = tabWidget_->labelFont();
	font.setPixelSize(14);
	tabWidget_->setLabelFont(font);

	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = session->getPages(pages);
	if (ec != Rpc::ec_success) {
		promptRpcError(ec);
		throw int(0);
	}

	Rpc::StringSeq contentCategories;
	ec = session->getContentCategories(contentCategories);
	if (ec != Rpc::ec_success) {
		promptRpcError(ec);
		throw int(0);
	}

	QStringList contentCategoryList;
	for (const std::string& s : contentCategories) {
		contentCategoryList << s.c_str();
	}

	QList<PageContentWidget*> pageContentWidgets;

	for (const std::string& page : pages)
	{
		const std::string::size_type pos = page.find(',');

		if (pos == std::string::npos)
		{
			std::string name = boost::erase_last_copy(page, "*");
			PageContentWidget* w = new PageContentWidget(context_, page.c_str());
			w->categoryFilterWidget()->labelSelectorWidget()->setLabels(contentCategoryList);
			tabWidget_->addTab(name.c_str(), w);
			pageContentWidgets.append(w);
			QObject::connect(w, &PageContentWidget::unresolvedUrl, this, &BaseClient::openUrl);
		}
		else {
			std::string name = page.substr(0, pos);
			std::string url = page.substr(pos + 1);
			PageWebWidget* w = new PageWebWidget(context_, name.c_str());
			w->setUrl(QString::fromStdString(url));
			tabWidget_->addTab(name.c_str(), w);
		}
	}

	for (int i = 0; i < pageContentWidgets.count(); ++i)
	{
		CategoryFilterWidget* w = pageContentWidgets[i]->categoryFilterWidget();
		for (int j = 0; j < pageContentWidgets.count(); ++j)
		{
			CategoryFilterWidget* w2 = pageContentWidgets[j]->categoryFilterWidget();
			QObject::connect(w, &CategoryFilterWidget::collapsed, w2, &CategoryFilterWidget::collapse);
			QObject::connect(w, &CategoryFilterWidget::extended, w2, &CategoryFilterWidget::extend);
		}
	}

	tabWidget_->addTab(QString::fromLocal8Bit("引擎"), new PageEngineWidget(context_, "Engine"));

	PageExtraWidget* pageExtraWidget = new PageExtraWidget(context_, "Extra");
	{
		Rpc::StringSeq extraCategories;
		ec = session->getExtraCategories(extraCategories);
		if (ec != Rpc::ec_success) {
			promptRpcError(ec);
			throw int(0);
		}

		QStringList extraCategoryList;
		for (const std::string& s : extraCategories) {
			extraCategoryList << s.c_str();
		}

		CategoryFilterWidget* w = pageExtraWidget->categoryFilterWidget();
		w->labelSelectorWidget()->setLabels(extraCategoryList);
		QObject::connect(w, &CategoryFilterWidget::collapsed, w, &CategoryFilterWidget::collapse);
		QObject::connect(w, &CategoryFilterWidget::extended, w, &CategoryFilterWidget::extend);
		tabWidget_->addTab(QString::fromLocal8Bit("安装包"), pageExtraWidget);
	}

	library_ = new LibraryWidget(context_);
	tabWidget_->addTab(QString::fromLocal8Bit("本地库"), library_);

	manage_ = new ManageWidget(context_);
	tabWidget_->addTab(QString::fromLocal8Bit("管理"), manage_);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(tabWidget_);
	layout->addWidget(lowerPane_);
	QWidget* w = new QWidget;
	w->setLayout(layout);
	setCentralWidget(w);

	QObject::connect(localServer_, &QLocalServer::newConnection, this, &BaseClient::onNewConnection);

	QObject::connect(decoratorWidgetUi_.listButton, &QPushButton::clicked, this, &BaseClient::onShowHistory);

	QObject::connect(taskManagerDialog_, &ASyncTaskManagerDialog::cleared, lowerPane_, &LowerPaneWidget::clear);

	if (!url.isEmpty()) {
		QMetaObject::invokeMethod(this, "openUrl", Qt::QueuedConnection, Q_ARG(QString, url));
	}

	QTimer* refreshTaskTimer = new QTimer(this);

	refreshTaskTimer->setInterval(100);
	refreshTaskTimer->start();

	QObject::connect(refreshTaskTimer, &QTimer::timeout, this, &BaseClient::onRefreshTasks);

	QTimer* checkNewContentsTimer = new QTimer(this);

	checkNewContentsTimer->setInterval(10000);
	checkNewContentsTimer->start();

	QObject::connect(checkNewContentsTimer, &QTimer::timeout, this, &BaseClient::onCheckNewContents);

	onCheckNewContents();
}

BaseClient::~BaseClient()
{
}

void BaseClient::closeEvent(QCloseEvent* e)
{
	size_t taskCount = 0;
	{
		boost::recursive_mutex::scoped_lock lock(taskListSync_);
		taskCount = taskList_.size();
	}

	if (taskCount)
	{
		int rc = QMessageBox::question(this, "Base",
			"One or more tasks are running in the background, do you want to quit anyway ?",
			QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

		if (rc == QMessageBox::No)
		{
			e->ignore();
			return;
		}
	}
}

void BaseClient::addTask(ASyncTaskPtr task)
{
	boost::recursive_mutex::scoped_lock lock(taskListSync_);

	taskList_.push_back(task);

	task->start();

	lowerPane_->addTask(task);
	taskManagerDialog_->listWidget()->addTask(task);
}

int64_t BaseClient::getLastViewStamp()
{
	std::ostringstream oss;

	oss << "SELECT Value FROM Infos";
	oss << " WHERE Key = 'LastViewStamp'";

	SQLite::Statement s(*db_, oss.str());

	if (!s.executeStep()) {
		return 0;
	}

	return std::stoll(s.getColumn("Value").getText());
}

void BaseClient::setLastViewStamp(int64_t stamp)
{
	{
		std::ostringstream oss;
		oss << "INSERT OR REPLACE INTO Infos VALUES ('LastViewStamp', ";
		oss << "'" << stamp << "'";
		oss << ")";

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();
	}

	onCheckNewContents();
}

std::string BaseClient::uniquePath()
{
	fs::path path = fs::temp_directory_path() / boost::uuids::to_string(rand_());

	return path.string();
}

std::string BaseClient::workPath()
{
	return workPath_;
}

std::string BaseClient::cachePath()
{
	fs::path path = fs::path(workPath()) / "Cache";

	return path.string();
}

std::string BaseClient::databasePath()
{
	fs::path path = fs::path(workPath()) / "Databases" / context_->currentUser;

	return path.string();
}

std::string BaseClient::outputPath()
{
	fs::path path = fs::path(workPath()) / "Outputs" / context_->currentUser;

	return path.string();
}

std::string BaseClient::enginePath(const EngineVersion& v)
{
	fs::path path = fs::path(workPath()) / "Engines" / context_->currentUser;

	path /= boost::to_lower_copy(
		boost::replace_all_copy(v.first, " ", "_") +
		"-" +
		boost::replace_all_copy(v.second, " ", "_")
		);

	return path.string();
}

std::string BaseClient::contentPath(const std::string& id)
{
	fs::path path = fs::path(workPath()) / "Contents" / context_->currentUser / id;

	return path.string();
}

std::string BaseClient::extraPath(const std::string& id)
{
	fs::path path = fs::path(workPath()) / "Extras" / context_->currentUser / id;

	return path.string();
}

void BaseClient::installEngine(const EngineVersion& v)
{
	int state = EngineState::not_installed;

	if (!changeEngineState(v, state, EngineState::installing)) {
		promptEngineState(v, state);
		return;
	}

	Rpc::ErrorCode ec;

	Rpc::DownloaderPrx downloader;
	if ((ec = context_->session->downloadEngineVersion(v.first, v.second, downloader)) != Rpc::ec_success) {
		state = EngineState::installing;
		changeEngineState(v, state, EngineState::not_installed);
		promptRpcError(ec);
		return;
	}

	boost::shared_ptr<ASyncInstallEngineTask> task(new ASyncInstallEngineTask(context_, downloader));

	task->setInfoHead("Install " + v.first + " " + v.second);
	task->setEngineVersion(v);
	task->setPath(context_->enginePath(v));

	addTask(task);
}

void BaseClient::setupEngine(const EngineVersion& v)
{
	Rpc::ErrorCode ec;

	Rpc::EngineVersionInfo info;
	if ((ec = context_->session->getEngineVersion(v.first, v.second, info)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	QStringList args = parseCombinedArgString(info.setup.c_str());
	if (args.isEmpty()) {
		return;
	}

	QString program = args.first();
	args.removeFirst();

	fs::path stdOutputFilename = fs::path(outputPath()) / (v.first + "-" + v.second + "-Setup.txt");

	QProcess p;

	p.setWorkingDirectory(QString::fromLocal8Bit(enginePath(v).c_str()));
	p.setProcessChannelMode(QProcess::MergedChannels);
	p.setStandardOutputFile(QString::fromLocal8Bit(stdOutputFilename.string().c_str()));
	p.start(program, args);

	if (!p.waitForStarted(-1)) {
		context_->prompt(2, "Setup program of " + v.first + " " + v.second + " has failed to start.\n"
			+ toLocal8bit(p.errorString()) + "\n" +
			"As a result, this Engine may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}

	p.waitForFinished(-1);

	if (p.exitStatus() != QProcess::NormalExit) {
		context_->prompt(1, "Setup program of " + v.first + " " + v.second + " not exited normally.\n"
			"As a result, this Engine may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}

	if (p.exitCode() != 0) {
		context_->prompt(1, "Setup program of " + v.first + " " + v.second + " returned non-zero exit code.\n"
			"As a result, this Engine may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}
}

void BaseClient::unSetupEngine(const EngineVersion& v)
{
	Rpc::ErrorCode ec;

	Rpc::EngineVersionInfo info;
	if ((ec = context_->session->getEngineVersion(v.first, v.second, info)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	QStringList args = parseCombinedArgString(info.unsetup.c_str());
	if (args.isEmpty()) {
		return;
	}

	QString program = args.first();
	args.removeFirst();

	fs::path stdOutputFilename = fs::path(outputPath()) / (v.first + "-" + v.second + "-UnSetup.txt");

	QProcess p;

	p.setWorkingDirectory(QString::fromLocal8Bit(enginePath(v).c_str()));
	p.setProcessChannelMode(QProcess::MergedChannels);
	p.setStandardOutputFile(QString::fromLocal8Bit(stdOutputFilename.string().c_str()));
	p.start(program, args);
	p.waitForFinished(-1);
}

void BaseClient::removeEngine(const EngineVersion& v)
{
	int state = EngineState::installed;

	if (!changeEngineState(v, state, EngineState::removing)) {
		promptEngineState(v, state);
		return;
	}

	removeEngineFromGui(v);

	boost::shared_ptr<ASyncRemoveEngineTask> task(new ASyncRemoveEngineTask(context_));
	task->setInfoHead("Remove " + v.first + " " + v.second);
	task->setEngineVersion(v);

	addTask(task);
}

int BaseClient::getEngineState(const EngineVersion& v)
{
	boost::recursive_mutex::scoped_lock lock(engineStateTabelSync_);

	if (engineStateTabel_.count(v) == 0) {
		engineStateTabel_.insert(std::make_pair(v, EngineState::not_installed));
	}

	return engineStateTabel_[v];
}

bool BaseClient::changeEngineState(const EngineVersion& v, int& oldState, int newState)
{
	boost::recursive_mutex::scoped_lock lock(engineStateTabelSync_);

	const int state = getEngineState(v);
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
		oss << sqlText(v.first) << ", ";
		oss << sqlText(v.second) << ")";

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);
		installedEngineTabel_.insert(v);
	}
	else if (newState == EngineState::removing)
	{
		std::ostringstream oss;
		oss << "DELETE FROM InstalledEngines";
		oss << " WHERE Name=";
		oss << sqlText(v.first);
		oss << " AND Version=";
		oss << sqlText(v.second);

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);
		installedEngineTabel_.erase(v);
	}

	engineStateTabel_[v] = newState;

	return true;
}

void BaseClient::getEngineList(std::vector<EngineVersion>& outList)
{
	outList.clear();
	boost::recursive_mutex::scoped_lock lock(installedEngineTabelSync_);
	for (const EngineVersion& v : installedEngineTabel_) {
		outList.push_back(v);
	}
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

		boost::recursive_mutex::scoped_lock lock(downloadedContentTabelSync_);
		downloadedContentTabel_.erase(id);
	}

	contentStateTabel_[id] = newState;

	return true;
}

void BaseClient::getDownloadedExtraList(std::vector<std::string>& outList)
{
	outList.clear();
	boost::recursive_mutex::scoped_lock lock(downloadedExtraTabelSync_);
	for (const std::string& id : downloadedExtraTabel_) {
		outList.push_back(id);
	}
}

void BaseClient::installExtra(const std::string& id)
{
	Rpc::ErrorCode ec;

	Rpc::ExtraInfo info;
	if ((ec = context_->session->getExtraInfo(id, info)) != Rpc::ec_success) {
		promptRpcError(ec);
		return;
	}

	int state = ExtraState::not_downloaded;

	if (!changeExtraState(id, state, ExtraState::downloading)) {
		promptExtraState(toLocal8bit(info.title), state);
		return;
	}

	Rpc::DownloaderPrx downloader;
	if ((ec = context_->session->downloadExtra(id, downloader)) != Rpc::ec_success) {
		promptRpcError(ec);
		return;
	}

	boost::shared_ptr<ASyncInstallExtraTask> task(new ASyncInstallExtraTask(context_, downloader));

	task->setInfoHead("Install " + toLocal8bit(info.title));
	task->setId(id);
	task->setPath(extraPath(id));

	addTask(task);
}

void BaseClient::setupExtra(const std::string& id)
{
	Rpc::ErrorCode ec;

	Rpc::ExtraInfo info;
	if ((ec = context_->session->getExtraInfo(id, info)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	std::string setup = toLocal8bit(info.setup);
	boost::replace_all(setup, "$(ExtraDir)", context_->extraPath(id));

	QStringList args = parseCombinedArgString(QString::fromLocal8Bit(setup.c_str()));
	if (args.isEmpty()) {
		return;
	}

	QString program = args.first();
	args.removeFirst();

	boost::replace_all(info.title, "\r", " ");

	fs::path stdOutputFilename = fs::path(outputPath()) / (toLocal8bit(info.title) + "-Setup.txt");

	QProcess p;

	p.setWorkingDirectory(QString::fromLocal8Bit(extraPath(id).c_str()));
	p.setProcessChannelMode(QProcess::MergedChannels);
	p.setStandardOutputFile(QString::fromLocal8Bit(stdOutputFilename.string().c_str()));
	p.start(program, args);

	if (!p.waitForStarted(-1)) {
		context_->prompt(2, "Setup program of " + toLocal8bit(info.title) + " has failed to start.\n"
			+ toLocal8bit(p.errorString()) + "\n" +
			"As a result, this Extra may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}

	p.waitForFinished(-1);

	if (p.exitStatus() != QProcess::NormalExit) {
		context_->prompt(1, "Setup program of " + toLocal8bit(info.title) + " not exited normally.\n"
			"As a result, this Extra may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}

	if (p.exitCode() != 0) {
		context_->prompt(1, "Setup program of " + toLocal8bit(info.title) + " returned non-zero exit code.\n"
			"As a result, this Extra may not function correctly.\n"
			"Please contact administrator for help.");
		return;
	}
}

int BaseClient::getExtraState(const std::string& id)
{
	boost::recursive_mutex::scoped_lock lock(extraStateTabelSync_);

	if (extraStateTabel_.count(id) == 0) {
		extraStateTabel_.insert(std::make_pair(id, ExtraState::not_downloaded));
	}

	return extraStateTabel_[id];
}

bool BaseClient::changeExtraState(const std::string& id, int& oldState, int newState)
{
	boost::recursive_mutex::scoped_lock lock(extraStateTabelSync_);

	const int state = getExtraState(id);
	if (state != oldState) {
		oldState = state;
		return false;
	}

	if (newState == oldState) {
		return true;
	}

	if (newState == ExtraState::downloaded)
	{
		std::ostringstream oss;
		oss << "INSERT INTO DownloadedExtras VALUES (";
		oss << sqlText(id) << ")";

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(downloadedExtraTabelSync_);
		downloadedExtraTabel_.insert(id);
	}
	else if (newState == ExtraState::not_downloaded)
	{
		std::ostringstream oss;
		oss << "DELETE FROM DownloadedExtras";
		oss << " WHERE Id=";
		oss << sqlText(id);

		SQLite::Transaction t(*db_);
		db_->exec(oss.str());
		t.commit();

		boost::recursive_mutex::scoped_lock lock(downloadedExtraTabelSync_);
		downloadedExtraTabel_.erase(id);
	}

	extraStateTabel_[id] = newState;

	return true;
}

void BaseClient::createProject(const std::string& contentId, const std::string& title, const std::string& location)
{
	Rpc::ContentInfo ci;
	Rpc::ErrorCode ec = context_->session->getContentInfo(contentId, ci);
	if (ec != Rpc::ec_success) {
		return;
	}

	std::vector<std::string> versions;
	boost::split(versions, ci.engineVersion, boost::is_any_of("|"));

	if (versions.empty()) {
		return;
	}

	std::map<std::string, std::string> properties;
	properties["name"] = title;
	properties["default_engine_version"] = ci.engineName + "\n" + versions.at(0);
	properties["startup"] = toLocal8bit(ci.startup);

	boost::shared_ptr<ASyncCreateProjectTask> task(new ASyncCreateProjectTask(context_));
	task->setInfoHead("Create " + toLocal8bit(ci.title));
	task->setContentId(contentId);
	task->setProjectId(boost::uuids::to_string(rand_()));
	task->setLocation(location);
	task->setProperties(properties);

	addTask(task);
}

void BaseClient::addProject(const std::string& id, const std::string& contentId, const std::string& location, const std::map<std::string, std::string>& properties)
{
	std::ostringstream oss;
	oss << "INSERT INTO Projects VALUES (";
	oss << sqlText(id) << ", ";
	oss << sqlText(contentId) << ", ";
	oss << sqlText(fromLocal8bit(location)) << ", ";
	oss << sqlText(fromLocal8bit(properties.at("name"))) << ", ";
	oss << sqlText(properties.at("default_engine_version")) << ", ";
	oss << sqlText(fromLocal8bit(properties.at("startup"))) << ")";

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	ProjectInfo pi;
	pi.id = id;
	pi.contentId = contentId;
	pi.location = location;
	pi.name = properties.at("name");
	pi.defaultEngineVersion = properties.at("default_engine_version");
	pi.startup = fromLocal8bit(properties.at("startup"));

	boost::unique_lock<boost::recursive_mutex> lock(projectTabelSync_);
	projectTabel_.insert(std::make_pair(id, pi));
	lock.unlock();
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

void BaseClient::changeProjectDefaultEngineVersion(const std::string& id, const EngineVersion& v)
{
	std::ostringstream oss;
	oss << "UPDATE Projects SET DefaultEngineVersion=";
	oss << sqlText(v.first + "\n" + v.second);
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	auto it = projectTabel_.find(id);
	if (it != projectTabel_.end()) {
		it->second.defaultEngineVersion = v.first + "\n" + v.second;
	}
}

void BaseClient::changeProjectStartup(const std::string& id, const std::string& startup)
{
	std::ostringstream oss;
	oss << "UPDATE Projects SET Startup=";
	oss << sqlText(startup);
	oss << " WHERE Id=";
	oss << sqlText(id);

	SQLite::Transaction t(*db_);
	db_->exec(oss.str());
	t.commit();

	boost::recursive_mutex::scoped_lock lock(projectTabelSync_);
	auto it = projectTabel_.find(id);
	if (it != projectTabel_.end()) {
		it->second.startup = startup;
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

void BaseClient::openUrl(const QString& url)
{
	if (url.startsWith("base:", Qt::CaseInsensitive))
	{
		std::string path;
		KVMap args;

		if (parseUrl(url.toStdString(), path, args))
		{
			if (path == "base://content/")
			{
				std::string page;

				if (args.lookupValue(page, "page"))
				{
					page = base64url_decode(page);

					for (int i = 0; i < tabWidget_->count(); ++i)
					{
						PageContentWidget* w = qobject_cast<PageContentWidget*>(tabWidget_->widget(i));

						if (w && w->name().toStdString() == page)
						{
							if (w->openUrl(url))
							{
								const int index = tabWidget_->indexOf(w);
								tabWidget_->setCurrentIndex(index);

								if (windowState() == Qt::WindowMinimized) {
									showNormal();
								}

								activateWindow();
							}
						}
					}
				}
			}
			else if (path == "base://extra/")
			{
				for (int i = 0; i < tabWidget_->count(); ++i)
				{
					PageExtraWidget* w = qobject_cast<PageExtraWidget*>(tabWidget_->widget(i));

					if (w)
					{
						if (w->openUrl(url))
						{
							const int index = tabWidget_->indexOf(w);
							tabWidget_->setCurrentIndex(index);

							if (windowState() == Qt::WindowMinimized) {
								showNormal();
							}
							
							activateWindow();
						}
					}
				}
			}
		}
	}
	else if (url.startsWith("http:", Qt::CaseInsensitive) || url.startsWith("https:", Qt::CaseInsensitive))
	{
		int index = 0;

		for (; index < tabWidget_->count(); ++index)
		{
			PageWebWidget* w = qobject_cast<PageWebWidget*>(tabWidget_->widget(tabWidget_->count() - index - 1));

			if (w)
			{
				w->setUrl(url);
				const int index = tabWidget_->indexOf(w);
				tabWidget_->setCurrentIndex(index);
				activateWindow();
				break;
			}
		}

		if (index == tabWidget_->count()) {
			QDesktopServices::openUrl(url);
		}
	}
}

void BaseClient::addEngineToGui(const EngineVersion& v)
{
	library_->addEngine(v.first.c_str(), v.second.c_str());
	addLibraryNotification();
}

void BaseClient::removeEngineFromGui(const EngineVersion& v)
{
	library_->removeEngine(v.first.c_str(), v.second.c_str());
}

void BaseClient::addContentToGui(const std::string& contentId)
{
	library_->addContent(contentId.c_str());
	addLibraryNotification();
}

void BaseClient::removeContentFromGui(const std::string& contentId)
{
	library_->removeContent(contentId.c_str());
}

void BaseClient::addProjectToGui(const std::string& projectId)
{
	library_->addProject(projectId.c_str());
	addLibraryNotification();
}

void BaseClient::removeProjectFromGui(const std::string& projectId)
{
	library_->removeProject(projectId.c_str());
}

void BaseClient::addExtraToGui(const std::string& id)
{
	library_->addExtra(id.c_str());
	addLibraryNotification();
}

void BaseClient::removeExtraFromGui(const std::string& id)
{
	library_->removeExtra(id.c_str());
}

void BaseClient::prompt(int level, const std::string& message)
{
	if (level <= 0) {
		QMessageBox::information(this, "Base", QString::fromLocal8Bit(message.c_str()));
	}
	else if (level == 1) {
		QMessageBox::warning(this, "Base", QString::fromLocal8Bit(message.c_str()));
	}
	else {
		QMessageBox::critical(this, "Base", QString::fromLocal8Bit(message.c_str()));
	}
}

void BaseClient::promptRpcError(Rpc::ErrorCode ec)
{
	if (ec == Rpc::ec_success) {
		QMessageBox::information(this, "Base", errorMessage(ec));
	}
	else {
		QMessageBox::critical(this, "Base", errorMessage(ec));
	}
}

void BaseClient::promptEngineState(const EngineVersion& v, int state)
{
	QString engine = QString("%1 %2").arg(v.first.c_str()).arg(v.second.c_str());
	QString message;

	if (state == EngineState::not_installed) {
		message = engine + " is not installed";
	}
	else if (state == EngineState::installing) {
		message = engine + " is now installing.";
	}
	else if (state == EngineState::installed) {
		message = engine + " is installed.";
	}
	else if (state == EngineState::configuring) {
		message = engine + " is now configuring.";
	}
	else if (state == EngineState::removing) {
		message = engine + " is now removing.";
	}

	QMessageBox::information(this, "Base", message);
}

void BaseClient::promptExtraState(const std::string& title, int state)
{
	QString extra = QString::fromLocal8Bit(title.c_str());
	QString message;

	if (state == ExtraState::not_downloaded) {
		message = extra + " is not downloaded";
	}
	else if (state == ExtraState::downloading) {
		message = extra + " is now downloading.";
	}
	else if (state == ExtraState::downloaded) {
		message = extra + " is downloaded.";
	}
	else if (state == ExtraState::configuring) {
		message = extra + " is now configuring.";
	}

	QMessageBox::information(this, "Base", message);
}

void BaseClient::addLibraryNotification()
{
	int index = tabWidget_->indexOf(library_);
	if (tabWidget_->currentIndex() != index) {
		tabWidget_->addNotification(index);
	}
}

void BaseClient::onShowHistory()
{
	if (historyDialog_->isHidden()) {
		historyDialog_->show();
	}
	else if (!historyDialog_->isActiveWindow()) {
		QApplication::setActiveWindow(historyDialog_);
	}
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

void BaseClient::onNewConnection()
{
	QLocalSocket* socket = localServer_->nextPendingConnection();

	socket->setReadBufferSize(4096);

	if (socket->waitForReadyRead(1000))
	{
		QDataStream in(socket);
		in.setVersion(QDataStream::Qt_5_5);

		QString url;
		in >> url;

		openUrl(url);
	}

	socket->deleteLater();
}

void BaseClient::onRefreshTasks()
{
	boost::recursive_mutex::scoped_lock lock(taskListSync_);

	for (auto it = taskList_.begin(); it != taskList_.end(); )
	{
		switch ((*it)->state())
		{
		case ASyncTask::state_cancelled:
		case ASyncTask::state_finished:
		case ASyncTask::state_failed:
			it = taskList_.erase(it);
			break;

		default:
			++it;
			break;
		}
	}
}

void BaseClient::onCheckNewContents()
{
	Rpc::ContentBrowserPrx browser;

	if (context_->session->browseContent("", "", "", browser) != Rpc::ec_success) {
		return;
	}

	int64_t lastViewStamp = getLastViewStamp();

	while (browser)
	{
		Rpc::ContentItemSeq items;

		if (browser->next(20, items) != Rpc::ec_success) {
			return;
		}

		if (items.size() < 20) {
			browser = 0;
		}

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (items[i].state == "Normal")
			{
				if (items[i].rowid > lastViewStamp) {
					decoratorWidgetUi_.listButton->setIcon(QIcon(":/Icons/ListNew.png"));
				}
				else {
					decoratorWidgetUi_.listButton->setIcon(QIcon(":/Icons/List.png"));
				}

				return;
			}
		}
	}
}

void BaseClient::initDb()
{
	fs::path p = fs::path(databasePath()) / "BaseClient.db";

	db_.reset(new SQLite::Database(fromLocal8bit(p.string()).c_str(), SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));

	db_->exec("CREATE TABLE IF NOT EXISTS InstalledEngines ("
		"Name TEXT COLLATE NOCASE, Version TEXT COLLATE NOCASE)");

	db_->exec("CREATE TABLE IF NOT EXISTS DownloadedContents ("
		"Id TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS DownloadedExtras ("
		"Id TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Projects ("
		"Id TEXT, ContentId TEXT, Location TEXT, Name TEXT, DefaultEngineVersion TEXT, Startup TEXT)");

	db_->exec("CREATE TABLE IF NOT EXISTS Infos (Key TEXT UNIQUE, Value TEXT)");
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
		installedEngineTabel_.insert(EngineVersion(name, version));
	}

	boost::recursive_mutex::scoped_lock lock2(engineStateTabelSync_);

	for (const EngineVersion& v : installedEngineTabel_) {
		engineStateTabel_[v] = EngineState::installed;
	}
}

void BaseClient::loadDownloadedExtrasFromDb()
{
	std::ostringstream oss;
	oss << "SELECT * FROM DownloadedExtras";

	boost::recursive_mutex::scoped_lock lock(downloadedExtraTabelSync_);

	SQLite::Statement s(*db_, oss.str());
	while (s.executeStep()) {
		downloadedExtraTabel_.insert(s.getColumn("Id").getText());
	}

	boost::recursive_mutex::scoped_lock lock2(extraStateTabelSync_);

	for (const std::string& id : downloadedExtraTabel_) {
		extraStateTabel_[id] = ExtraState::downloaded;
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
		pi.defaultEngineVersion = s.getColumn("DefaultEngineVersion").getText();
		pi.startup = toLocal8bit(s.getColumn("Startup").getText());
		projectTabel_.insert(std::make_pair(pi.id, pi));
	}
}

