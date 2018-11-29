#include "HTabBarTabWidget.h"

#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>

HTabBarTabWidget::HTabBarTabWidget(QWidget* parent) : QWidget(parent)
{
	font_ = font();

	notification_ = 0;

	setProperty("active", false);
}

HTabBarTabWidget::~HTabBarTabWidget()
{

}

void HTabBarTabWidget::setLabel(const QString& text)
{
	const int p = font_.pixelSize();
	const int l = text.size();

	const int width = qMax(p * (l + 1) + p / 2, p * 6);
	const int height = p * 3;

	setFixedSize(width, height);

	text_ = text;
}

void HTabBarTabWidget::setLabelFont(const QFont& font)
{
	font_ = font;
}

void HTabBarTabWidget::setActive(bool active)
{
	style()->unpolish(this);
	setProperty("active", active);
	style()->polish(this);
}

void HTabBarTabWidget::addNotification()
{
	++notification_;

	repaint();
}

void HTabBarTabWidget::clearNotification()
{
	notification_ = 0;

	repaint();
}

void HTabBarTabWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;

	opt.init(this);

	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);

	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	painter.setFont(font_);

	painter.drawText(rect(), Qt::AlignCenter, text_);

	if (notification_ > 0)
	{
		QFont f = font_;

		f.setPixelSize(9);

		painter.setFont(f);

		QRect bound;

		painter.drawText(rect().adjusted(0, 9, -9, 0), Qt::AlignRight|Qt::AlignTop, QString("%1").arg(notification_), &bound);

		if (bound.width() < bound.height())
		{
			int d = bound.height() - bound.width();
			bound.adjust(-d / 2, 0, d / 2, 0);
		}

		QPainterPath path;

		path.addRoundedRect(bound, 6, 6);

		painter.fillPath(path, QColor(246, 96, 0));

		painter.drawText(rect().adjusted(0, 9, -9, 0), Qt::AlignRight|Qt::AlignTop, QString("%1").arg(notification_));
	}
}

