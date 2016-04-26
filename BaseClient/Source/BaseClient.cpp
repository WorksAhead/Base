#include "BaseClient.h"

#include <QTabBar>
#include <QTextEdit>

BaseClient::BaseClient()
{
	setWindowIcon(QIcon(":/Icons/BaseClient.png"));
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
