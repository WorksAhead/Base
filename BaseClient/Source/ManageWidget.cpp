#include "ManageWidget.h"
#include "VTabWidget.h"
#include "ManageContentWidget.h"
#include "ManageEngineWidget.h"
#include "ManageExtraWidget.h"
#include "ManageUserWidget.h"
#include "ManageInfoWidget.h"
#include "ManageClientWidget.h"

#include <QBoxLayout>
#include <QPainter>
#include <QTextEdit>

ManageWidget::ManageWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	VTabWidget* p = new VTabWidget;

	QFont font = p->labelFont();
	font.setPixelSize(14);
	p->setLabelFont(font);

	manageContent_ = new ManageContentWidget(context_);
	p->addTab("Content", manageContent_);

	if (context_->currentUserGroup == "Admin")
	{
		manageEngine_ = new ManageEngineWidget(context_);
		manageExtra_ = new ManageExtraWidget(context_);
		manageUser_ = new ManageUserWidget(context_);
		manageInfoWidget_ = new ManageInfoWidget(context_);
		manageClient_ = new ManageClientWidget(context_);

		p->addTab("Engine", manageEngine_);
		p->addTab("Extra", manageExtra_);
		p->addTab("User", manageUser_);
		p->addTab("Info", manageInfoWidget_);
		p->addTab("Client", manageClient_);
	}

	p->setCurrentIndex(0);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 6, 0);
	layout->setSpacing(0);
	layout->addWidget(p);
	setLayout(layout);
}

ManageWidget::~ManageWidget()
{
}

void ManageWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

