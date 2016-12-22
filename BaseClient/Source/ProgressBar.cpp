#include "ProgressBar.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include <math.h>

ProgressBar::ProgressBar(QWidget* parent) : QWidget(parent)
{
	minimum_ = 0;
	maximum_ = 1;
	value_ = 0;
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::setRange(int minimum, int maximum)
{
	minimum_ = minimum;
	maximum_ = maximum;
}

void ProgressBar::setMinimum(int minimum)
{
	minimum_ = minimum;
}

void ProgressBar::setMaximum(int maximum)
{
	maximum_ = maximum;
}

void ProgressBar::setValue(int value)
{
	value_ = value;
	repaint();
}

void ProgressBar::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		const int val = (int)floor((double)minimum_ + (double)(maximum_ - minimum_) * ((double)e->pos().x() / (double)width()));
		Q_EMIT clicked(val);
	}
}

void ProgressBar::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);

	const int val = (int)floor(width() * ((double)(value_ - minimum_) / (double)(maximum_ - minimum_)) + 0.5);

	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing);

	QPainterPath path;
	path.addRoundedRect(QRect(0, 0, width(), height() - 1), 3, 3);

	p.fillPath(path, opt.palette.background().color());

	p.setClipping(true);
	p.setClipRect(0, 0, val, height());

	p.fillPath(path, opt.palette.foreground().color());
}

