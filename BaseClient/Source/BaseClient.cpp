#include "BaseClient.h"
#include "Manage.h"

#include <QTabBar>
#include <QTextEdit>

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

	context_.reset(new Context);
	context_->session = session;
	context_->addTask = std::bind(&ASyncTaskListWidget::addTask, taskManagerDialog_->listWidget(), std::placeholders::_1);

	QWidget* decoratorWidget = new QWidget;
	decoratorWidgetUi_.setupUi(decoratorWidget);

	setDecoratorWidget(decoratorWidget);

	manage_ = new Manage(context_);

	tab_ = new QTabWidget;
	tab_->setAutoFillBackground(true);
	tab_->setObjectName("MainTab");
	tab_->tabBar()->setObjectName("MainTabBar");

	QFont font = tab_->tabBar()->font();
	font.setPixelSize(16);
	tab_->tabBar()->setFont(font);

	tab_->addTab(manage_, "Manage");

	setCentralWidget(tab_);

	QObject::connect(decoratorWidgetUi_.taskButton, &QPushButton::clicked, taskManagerDialog_, &QDialog::show);

	timer_ = new IceUtil::Timer;
	timer_->scheduleRepeated(new RefreshTask(session), IceUtil::Time::seconds(5));
}

BaseClient::~BaseClient()
{
}

