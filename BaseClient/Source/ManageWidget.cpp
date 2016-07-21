#include "ManageWidget.h"
#include "VTabWidget.h"
#include "ManageEngineWidget.h"
#include "ManagePageWidget.h"
#include "ManageCategoryWidget.h"

#include <QBoxLayout>
#include <QTextEdit>

ManageWidget::ManageWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	VTabWidget* p = new VTabWidget;

	manageEngine_ = new ManageEngineWidget(context_);
	managePage_ = new ManagePageWidget(context_);
	manageCategory_ = new ManageCategoryWidget(context_);

	p->addTab("Engines", manageEngine_);
	p->addTab("Users", new QTextEdit);
	p->addTab("Pages", managePage_);
	p->addTab("Categories", manageCategory_);

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

