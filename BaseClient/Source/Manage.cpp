#include "Manage.h"
#include "VTabWidget.h"
#include "ManageEngine.h"
#include "ManagePage.h"

#include <QBoxLayout>
#include <QTextEdit>

Manage::Manage(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	VTabWidget* p = new VTabWidget;

	manageEngine_ = new ManageEngine(context_);
	managePage_ = new ManagePage(context_);

	p->addTab("Engines", manageEngine_);
	p->addTab("Users", new QTextEdit);
	p->addTab("Pages", managePage_);

	p->setCurrentIndex(0);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(p);
	setLayout(layout);
}

Manage::~Manage()
{
}

