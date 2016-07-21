#include "ImageViewerWidget.h"

#include <QGridLayout>
#include <QResizeEvent>

ImageViewerWidget::ImageViewerWidget(QWidget* parent) : QFrame(parent)
{
	label_ = new QLabel;
	label_->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(label_);
	setLayout(layout);
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

void ImageViewerWidget::resizeEvent(QResizeEvent* e)
{
	label_->setPixmap(pixmap_.scaled(e->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

