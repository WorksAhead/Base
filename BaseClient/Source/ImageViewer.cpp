#include "ImageViewer.h"

#include <QGridLayout>
#include <QResizeEvent>

ImageViewer::ImageViewer(QWidget* parent) : QFrame(parent)
{
	label_ = new QLabel;
	label_->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(label_);
	setLayout(layout);
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::setPixmap(const QPixmap& pixmap)
{
	pixmap_ = pixmap;
	label_->setPixmap(pixmap_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QPixmap ImageViewer::pixmap() const
{
	return pixmap_;
}

void ImageViewer::resizeEvent(QResizeEvent* e)
{
	label_->setPixmap(pixmap_.scaled(e->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

