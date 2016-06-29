#include "ImageCropperWidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <math.h>

ImageCropperWidget::ImageCropperWidget(QWidget* parent) : QWidget(parent)
{
	box_ = new CroppingBoxWidget(this);

	ratio_ = 0.0f;
}

ImageCropperWidget::~ImageCropperWidget()
{
}

void ImageCropperWidget::setRatio(const QSize& size)
{
	if (size.isEmpty()) {
		ratio_ = 0.0f;
		return;
	}

	ratio_ = (float)((double)size.width() / (double)size.height());

	if (ratio_ >= 1.0f) {
		box_->setMinimumSize(QSize(10 * ratio_, 10));
	}
	else {
		box_->setMinimumSize(QSize(10, 10 / ratio_));
	}

	box_->setRatio(size);
}

void ImageCropperWidget::setImage(const QPixmap& image)
{
	image_ = image;

	updateImageGeometry(size());

	box_->setArea(imageRect_);

	if (ratio_ != 0.0f)
	{
		int x = imageRect_.x();
		int y = imageRect_.y();
		int w = imageRect_.width();
		int h = imageRect_.height();
		if (w >= h) {
			w = h * ratio_;
		}
		else {
			h = w / ratio_;
		}
		box_->setGeometry(x, y, w, h);
	}
	else {
		box_->setGeometry(imageRect_);
	}
}

QPixmap ImageCropperWidget::cropImage()
{
	if (image_.isNull() || imageRect_.isNull()) {
		return QPixmap();
	}

	QRect rect = box_->geometry();

	rect.moveTopLeft(rect.topLeft() - imageRect_.topLeft());

	double s = (double)rect.width() / (double)imageRect_.width();
	double t = (double)rect.height() / (double)imageRect_.height();
	double u = (double)rect.x() / (double)imageRect_.width();
	double v = (double)rect.y() / (double)imageRect_.height();

	int x, y, w, h;

	if (ratio_ >= 1.0f) {
		w = (image_.width() * s);
		h = w / ratio_;
	}
	else if (ratio_ > 0.0f) {
		h = (image_.height() * t);
		w = h * ratio_;
	}
	else {
		w = (image_.width() * s);
		h = (image_.height() * t);
	}

	x = (image_.width() * u);
	y = (image_.height() * v);

	rect.setSize(QSize(w, h));
	rect.moveTopLeft(QPoint(x, y));

	return image_.copy(rect);
}

void ImageCropperWidget::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	if (!scaledImage_.isNull()) {
		painter.drawPixmap(imageRect_, scaledImage_);
	}
}

void ImageCropperWidget::resizeEvent(QResizeEvent* e)
{
	QRect oldImageRect;

	if (imageRect_.contains(box_->geometry())) {
		oldImageRect = imageRect_;
	}

	updateImageGeometry(e->size());

	if (oldImageRect.isValid())
	{
		QRect rect = box_->geometry();

		rect.moveTopLeft(rect.topLeft() - oldImageRect.topLeft());

		double s = (double)rect.width() / (double)oldImageRect.width();
		double t = (double)rect.height() / (double)oldImageRect.height();
		double u = (double)rect.x() / (double)oldImageRect.width();
		double v = (double)rect.y() / (double)oldImageRect.height();

		int x, y, w, h;

		if (ratio_ >= 1.0f) {
			w = (imageRect_.width() * s);
			h = w / ratio_;
		}
		else if (ratio_ > 0.0f) {
			h = (imageRect_.height() * t);
			w = h * ratio_;
		}
		else {
			w = (imageRect_.width() * s);
			h = (imageRect_.height() * t);
		}

		x = (imageRect_.width() * u);
		y = (imageRect_.height() * v);

		x += imageRect_.x();
		y += imageRect_.y();

		rect.setSize(QSize(w, h));
		rect.moveTopLeft(QPoint(x, y));

		box_->setGeometry(rect);
	}

	if (imageRect_.isValid()) {
		box_->setArea(imageRect_);
	}
	else {
		box_->setArea(rect());
	}

	if (imageRect_.isValid()) {
		box_->setArea(imageRect_);
	}
	else {
		box_->setArea(rect());
	}
}

void ImageCropperWidget::updateImageGeometry(const QSize& size)
{
	if (!image_.isNull())
	{
		scaledImage_ = image_.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		imageRect_.setSize(scaledImage_.size());

		if (scaledImage_.height() == size.height()) {
			imageRect_.moveTo(QPoint((size.width() - scaledImage_.width()) / 2, 0));
		}
		else {
			imageRect_.moveTo(QPoint(0, (size.height() - scaledImage_.height()) / 2));
		}
	}
}

