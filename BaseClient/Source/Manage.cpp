#include "Manage.h"
#include "VTabWidget.h"
#include "ManageEngine.h"

#include <QBoxLayout>
#include <QTextEdit>

Manage::Manage(Rpc::SessionPrx session, QWidget* parent) : QWidget(parent), session_(session)
{
	VTabWidget* p = new VTabWidget;

	manageEngine_ = new ManageEngine(session);

	p->addTab("Engines", manageEngine_);
	p->addTab("Users", new QTextEdit);
	p->addTab("Transfer", new QTextEdit);
	p->setCurrentIndex(1);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(p);
	setLayout(layout);
}

Manage::~Manage()
{
}

