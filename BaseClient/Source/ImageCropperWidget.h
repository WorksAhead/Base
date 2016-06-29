#ifndef IMAGECROPPERWIDGET_HEADER_
#define IMAGECROPPERWIDGET_HEADER_

#include "EdgeDetector.h"
#include "CroppingBoxWidget.h"

#include <QWidget>
#include <QPixmap>

class ImageCropperWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ImageCropperWidget(QWidget* parent = 0);
	~ImageCropperWidget();

	void setRatio(const QSize& size);
	void setImage(const QPixmap& image);

	QPixmap cropImage();

protected:
	virtual void paintEvent(QPaintEvent*);

	virtual void resizeEvent(QResizeEvent*);

private:
	void updateImageGeometry(const QSize& size);

private:
	CroppingBoxWidget* box_;

	QPixmap image_;
	QPixmap scaledImage_;
	QRect imageRect_;
	float ratio_;
};


#endif // IMAGECROPPERWIDGET_HEADER_

