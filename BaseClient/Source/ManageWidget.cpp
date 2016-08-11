#include "ManageWidget.h"
#include "VTabWidget.h"
#include "ManageContentWidget.h"
#include "ManageEngineWidget.h"
#include "ManageUserWidget.h"
#include "ManagePageWidget.h"
#include "ManageCategoryWidget.h"

#include <QBoxLayout>
#include <QTextEdit>

ManageWidget::ManageWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	VTabWidget* p = new VTabWidget;

	manageContent_ = new ManageContentWidget(context_);
	manageEngine_ = new ManageEngineWidget(context_);
	manageUser_ = new ManageUserWidget(context_);
	managePage_ = new ManagePageWidget(context_);
	manageCategory_ = new ManageCategoryWidget(context_);

	p->addTab("Content", manageContent_);
	p->addTab("Engine", manageEngine_);
	p->addTab("User", manageUser_);
	p->addTab("Page", managePage_);
	p->addTab("Category", manageCategory_);

	p->setCurrentIndex(0);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(p);
	setLayout(layout);
}

ManageWidget::~ManageWidget()
{
}

