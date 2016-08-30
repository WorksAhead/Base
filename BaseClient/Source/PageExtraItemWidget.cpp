#include "PageExtraItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>

PageExtraItemWidget::PageExtraItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QObject::connect(ui_.installButton, &QPushButton::clicked, this, &PageExtraItemWidget::onInstall);
}

PageExtraItemWidget::~PageExtraItemWidget()
{
}

void PageExtraItemWidget::setId(const QString& id)
{
	id_ = id;
}

void PageExtraItemWidget::setTitle(const QString& title)
{
	ui_.nameLabel->setText(title);
}

void PageExtraItemWidget::mousePressEvent(QMouseEvent*)
{
}

void PageExtraItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
}

void PageExtraItemWidget::resizeEvent(QResizeEvent*)
{
}

void PageExtraItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageExtraItemWidget::onInstall()
{
	context_->installExtra(id_.toStdString());
}

