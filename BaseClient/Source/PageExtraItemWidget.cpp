#include "PageExtraItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>

PageExtraItemWidget::PageExtraItemWidget(QWidget* parent) : QWidget(parent)
{
	size_ = 1;

	setSize(size_);
}

PageExtraItemWidget::~PageExtraItemWidget()
{
}

void PageExtraItemWidget::setId(const QString& id)
{
	id_ = id;
}

QString PageExtraItemWidget::id() const
{
	return id_;
}

void PageExtraItemWidget::setText(const QString& text)
{
	text_ = text;
}

QString PageExtraItemWidget::text() const
{
	return text_;
}

void PageExtraItemWidget::setBackground(const QPixmap& bg)
{
	bg_ = bg;
	updateBackground(size());
}

void PageExtraItemWidget::setSize(int size)
{
	size_ = size;

	if (size == 0) {
		setFixedSize(150, 150);
	}
	else {
		setFixedSize(230, 230);
	}
}

void PageExtraItemWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), scaledBg_);

	if (size_ == 0)
	{
		QFont f = font();
		f.setPixelSize(12);

		QFontMetrics fm(f);

		QRect textRect = fm.boundingRect(rect().adjusted(5, 5, -5, -5), Qt::AlignLeft|Qt::TextWordWrap, text_);
		textRect.moveBottom(rect().height() - 5);

		painter.fillRect(textRect.adjusted(-2, -2, 2, 2), QColor(0, 0, 0, 160));

		painter.setFont(f);
		painter.drawText(textRect, text_);
	}
	else {
		QFont f = font();
		f.setPixelSize(14);

		QFontMetrics fm(f);

		QRect textRect = fm.boundingRect(rect().adjusted(15, 15, -15, -15), Qt::AlignLeft|Qt::TextWordWrap, text_);
		textRect.moveBottom(rect().height() - 15);

		painter.fillRect(textRect.adjusted(-5, -5, 5, 5), QColor(0, 0, 0, 160));

		painter.setFont(f);
		painter.drawText(textRect, text_);
	}
}

void PageExtraItemWidget::resizeEvent(QResizeEvent* e)
{
	updateBackground(e->size());
}

void PageExtraItemWidget::updateBackground(const QSize& s)
{
	if (!bg_.isNull()) {
		scaledBg_ = bg_.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		repaint();
	}
}

