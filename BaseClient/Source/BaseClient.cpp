#include "BaseClient.h"

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

BaseClient::BaseClient()
{
	setWindowIcon(QIcon(":/Icons/Base20x20.png"));
	setWindowTitle("Base");

	tab_ = new QTabWidget;
	tab_->setAutoFillBackground(true);
	tab_->setObjectName("MainTab");
	tab_->tabBar()->setObjectName("MainTabBar");

	QFont font = tab_->tabBar()->font();
	font.setPixelSize(16);
	tab_->tabBar()->setFont(font);

	tab_->addTab(new QTextEdit, "UE4");
	tab_->addTab(new QTextEdit, "MTitan");
	tab_->addTab(new QTextEdit, "A very very long tab name");
	tab_->addTab(new QTextEdit, "Manager");

	setCentralWidget(tab_);	
}

BaseClient::~BaseClient()
{
}

void BaseClient::setSession(Rpc::SessionPrx session)
{
	timer_ = new IceUtil::Timer;
	timer_->scheduleRepeated(new RefreshTask(session), IceUtil::Time::seconds(5));
	session_ = session;
}

