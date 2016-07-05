#include "BaseClient.h"
#include "Page.h"
#include "Manage.h"
#include "ErrorMessage.h"

#include <QTabBar>
#include <QTextEdit>
#include <QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

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

	taskManagerDialog_ = new ASyncTaskManagerDialog(this);

	// todo
	if (!fs::exists("Cache") && !fs::create_directories("Cache")) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists("Library/Contents") && !fs::create_directories("Library/Contents")) {
		throw std::runtime_error("Failed to create directory");
	}
	if (!fs::exists("Library/Engines") && !fs::create_directories("Library/Engines")) {
		throw std::runtime_error("Failed to create directory");
	}

	context_.reset(new Context);
	context_->session = session;
	context_->addTask = std::bind(&ASyncTaskListWidget::addTask, taskManagerDialog_->listWidget(), std::placeholders::_1);
	context_->uniquePath = std::bind(&BaseClient::uniquePath, this);
	context_->cachePath = std::bind(&BaseClient::cachePath, this);
	context_->libraryPath = std::bind(&BaseClient::libraryPath, this);
	context_->contentPath = std::bind(&BaseClient::contentPath, this, std::placeholders::_1);
	context_->promptRpcError = std::bind(&BaseClient::promptRpcError, this, std::placeholders::_1);

	QWidget* decoratorWidget = new QWidget;
	decoratorWidgetUi_.setupUi(decoratorWidget);

	setDecoratorWidget(decoratorWidget);

	tab_ = new QTabWidget;
	tab_->setAutoFillBackground(true);
	tab_->setObjectName("MainTab");
	tab_->tabBar()->setObjectName("MainTabBar");

	QFont font = tab_->tabBar()->font();
	font.setPixelSize(16);
	tab_->tabBar()->setFont(font);

	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = session->getPages(pages);
	if (ec != Rpc::ec_success) {
		promptRpcError(ec);
		throw int(0);
	}

	for (const std::string& page : pages) {
		tab_->addTab(new Page(context_, page.c_str()), page.c_str());
	}

	manage_ = new Manage(context_);

	tab_->addTab(manage_, "Manage");

	setCentralWidget(tab_);

	QObject::connect(decoratorWidgetUi_.taskButton, &QPushButton::clicked, taskManagerDialog_, &QDialog::show);

	timer_ = new IceUtil::Timer;
	timer_->scheduleRepeated(new RefreshTask(session), IceUtil::Time::seconds(5));
}

BaseClient::~BaseClient()
{
}

std::string BaseClient::uniquePath()
{
	fs::path p = fs::temp_directory_path();
	p = p / boost::uuids::to_string(uniquePathGen_());
	return p.string();
}

std::string BaseClient::cachePath()
{
	return "Cache";
}

std::string BaseClient::libraryPath()
{
	// todo
	return "Library";
}

std::string BaseClient::contentPath(const std::string& id)
{
	fs::path path = libraryPath();
	path /= "Contents";
	path /= id;
	return path.string();
}

void BaseClient::promptRpcError(Rpc::ErrorCode ec)
{
	QMessageBox::critical(this, "Base", errorMessage(ec));
	return;
}

