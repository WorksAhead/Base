#include "ImageCropperWidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <math.h>

ImageCropperWidget::ImageCropperWidget(QWidget* parent) : QWidget(parent)
{
	box_ = new CroppingBoxWidget(this);

	imageAspectRatio_ = 0.0f;
}

ImageCropperWidget::~ImageCropperWidget()
{
}

void ImageCropperWidget::setImageAspectRatio(const QSize& size)
{
	if (size.isEmpty()) {
		imageAspectRatio_ = 0.0f;
		return;
	}

	imageAspectRatio_ = (float)((double)size.width() / (double)size.height());

	if (imageAspectRatio_ >= 1.0f) {
		box_->setMinimumSize(QSize(10 * imageAspectRatio_, 10));
	}
	else {
		box_->setMinimumSize(QSize(10, 10 / imageAspectRatio_));
	}

	box_->setRatio(size);
}

void ImageCropperWidget::setImage(const QPixmap& image)
{
	image_ = image;
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

	if (imageAspectRatio_ >= 1.0f) {
		w = (image_.width() * s);
		h = w / imageAspectRatio_;
	}
	else if (imageAspectRatio_ > 0.0f) {
		h = (image_.height() * t);
		w = h * imageAspectRatio_;
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
	updateImageGeometry(e->size());

	box_->setArea(imageRect_);

	QRect boxRect = imageRect_;

	if (imageAspectRatio_ != 0.0f)
	{
		float r = (double)imageRect_.width() / (double)imageRect_.height();

		if (r > imageAspectRatio_) {
			boxRect.setWidth(boxRect.height() * imageAspectRatio_);
		}
		else if (r < imageAspectRatio_) {
			boxRect.setHeight(boxRect.width() / imageAspectRatio_);
		}
	}

	box_->setGeometry(boxRect);
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

