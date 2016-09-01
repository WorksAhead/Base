#include "ImageViewerWidget.h"

#include <QGridLayout>
#include <QResizeEvent>
#include <QDebug>

ImageViewerWidget::ImageViewerWidget(QWidget* parent) : QFrame(parent)
{
	label_ = new QLabel;
	label_->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(label_);
	setLayout(layout);

	aspectRatio_ = 0.0;
}

ImageViewerWidget::~ImageViewerWidget()
{
}

void ImageViewerWidget::setPixmap(const QPixmap& pixmap)
{
	pixmap_ = pixmap;
	label_->setPixmap(pixmap_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QPixmap ImageViewerWidget::pixmap() const
{
	return pixmap_;
}

void ImageViewerWidget::setAspectRatio(float aspectRatio)
{
	aspectRatio_ = aspectRatio;
}

void ImageViewerWidget::resizeEvent(QResizeEvent* e)
{
	QSize size = e->size();
	if (aspectRatio_ != 0.0f) {
		int height = size.width() / aspectRatio_;
		if (size.height() != height) {
			size.setHeight(height);
			setFixedHeight(height);
		}
	}
	label_->setPixmap(pixmap_.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

