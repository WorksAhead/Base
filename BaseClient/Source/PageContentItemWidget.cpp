#include "PageContentItemWidget.h"

#include <QResizeEvent>
#include <QPainter>
#include <QFontMetrics>

PageContentItemWidget::PageContentItemWidget(QWidget* parent) : QWidget(parent)
{
	bg_ = 0;
	movie_ = 0;

	size_ = 1;

	setSize(size_);
}

PageContentItemWidget::~PageContentItemWidget()
{
	clear();
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

QString PageContentItemWidget::text() const
{
	return text_;
}

void PageContentItemWidget::setBackground(QPixmap* bg)
{
	clear();

	bg_ = bg;
	scaledBg_ = bg_->scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	repaint();
}

void PageContentItemWidget::setBackground(QMovie* movie)
{
	clear();

	movie_ = movie;

	QObject::connect(movie_, SIGNAL(updated(QRect)), this, SLOT(onBackgroundUpdated(QRect)));

	movie_->start();
}

bool PageContentItemWidget::hasBackground()
{
	return (bg_ || movie_);
}

void PageContentItemWidget::setSize(int size)
{
	size_ = size;

	if (size == 0) {
		setFixedSize(150, 150);
	}
	else {
		setFixedSize(230, 230);
	}
}

void PageContentItemWidget::onBackgroundUpdated(QRect)
{
	if (movie_ && movie_->isValid())
	{
		scaledBg_ = movie_->currentPixmap().scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

		repaint();
	}
}

void PageContentItemWidget::paintEvent(QPaintEvent* e)
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

void PageContentItemWidget::resizeEvent(QResizeEvent* e)
{
	if (bg_)
	{
		scaledBg_ = bg_->scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	}
}

void PageContentItemWidget::clear()
{
	if (bg_) {
		bg_ = 0;
	}

	if (movie_)
	{
		QObject::disconnect(movie_, SIGNAL(updated(QRect)), this, SLOT(onBackgroundUpdated(QRect)));
		movie_ = 0;
	}

	scaledBg_ = QPixmap();
}

