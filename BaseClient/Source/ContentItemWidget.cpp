#include "ContentItemWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>

ContentItemWidget::ContentItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);
	QMenu* menu = new QMenu;
	menu->addAction("Remove");
	ui_.createButton->setMenu(menu);
}

ContentItemWidget::~ContentItemWidget()
{
}

void ContentItemWidget::setContentId(const QString& id)
{
	contentId_ = id;
}

const QString& ContentItemWidget::contentId() const
{
	return contentId_;
}

void ContentItemWidget::setTitle(const QString& text)
{
	ui_.titleLabel->setText(text);
}

void ContentItemWidget::setImage(const QPixmap& pixmap)
{
	ui_.thumbnailViewer->setPixmap(pixmap);
}

void ContentItemWidget::mousePressEvent(QMouseEvent*)
{
}

void ContentItemWidget::resizeEvent(QResizeEvent*)
{
}

void ContentItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

