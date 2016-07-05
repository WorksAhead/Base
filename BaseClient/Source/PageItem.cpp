#include "PageItem.h"

#include <QResizeEvent>
#include <QPainter>
#include <QFontMetrics>

PageItem::PageItem(QWidget* parent) : QWidget(parent)
{
}

PageItem::~PageItem()
{
}

void PageItem::setId(const QString& id)
{
	id_ = id;
}

QString PageItem::id() const
{
	return id_;
}

void PageItem::setText(const QString& text)
{
	text_ = text;
}

void PageItem::setBackground(const QPixmap& bg)
{
	bg_ = bg;
	updateBackground(size());
}

void PageItem::paintEvent(QPaintEvent* e)
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

void PageItem::resizeEvent(QResizeEvent* e)
{
	updateBackground(e->size());
}

void PageItem::updateBackground(const QSize& s)
{
	if (!bg_.isNull()) {
		scaledBg_ = bg_.scaled(s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		repaint();
	}
}

