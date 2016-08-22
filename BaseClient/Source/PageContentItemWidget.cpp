#include "PageContentItemWidget.h"

#include <QResizeEvent>
#include <QPainter>
#include <QFontMetrics>

PageContentItemWidget::PageContentItemWidget(QWidget* parent) : QWidget(parent)
{
}

PageContentItemWidget::~PageContentItemWidget()
{
}

void PageContentItemWidget::setId(const QString& id)
{
	id_ = id;
}

QString PageContentItemWidget::id() const
{
	return id_;
}

void PageContentItemWidget::setText(const QString& text)
{
	text_ = text;
}

void PageContentItemWidget::setBackground(const QPixmap& bg)
{
	bg_ = bg;
	updateBackground(size());
}

void PageContentItemWidget::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), scaledBg_);
	
	QFont f = font();
	f.setPixelSize(14);

	QFontMetrics fm(f);

	QRect textRect = fm.boundingRect(rect().adjusted(15, 15, -15, -15), Qt::AlignLeft|Qt::TextWordWrap, text_);
	textRect.moveBottom(rect().height() - 15);

	painter.fillRect(textRect.adjusted(-5, -5, 5, 5), QColor(0, 0, 0, 160));

	painter.setFont(f);
	painter.drawText(textRect, text_);
}

void PageContentItemWidget::resizeEvent(QResizeEvent* e)
{
	updateBackground(e->size());
}

void PageContentItemWidget::updateBackground(const QSize& s)
{
	if (!bg_.isNull()) {
		scaledBg_ = bg_.scaled(s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		repaint();
	}
}

