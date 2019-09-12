#include "PageEngineItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>

PageEngineItemWidget::PageEngineItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QObject::connect(ui_.installButton, &QPushButton::clicked, this, &PageEngineItemWidget::onInstall);
}

PageEngineItemWidget::~PageEngineItemWidget()
{
}

void PageEngineItemWidget::setEngineVersion(const QPair<QString, QString>& v)
{
	engineVersion_ = v;
	ui_.nameLabel->setText(v.first + " " + v.second);
}

QPair<QString, QString> PageEngineItemWidget::getEngineVersion()
{
	return engineVersion_;
}

void PageEngineItemWidget::mousePressEvent(QMouseEvent*)
{
}

void PageEngineItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
}

void PageEngineItemWidget::resizeEvent(QResizeEvent*)
{
}

void PageEngineItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageEngineItemWidget::onInstall()
{
	EngineVersion v(engineVersion_.first.toStdString(), engineVersion_.second.toStdString());
	context_->installEngine(v);
}

