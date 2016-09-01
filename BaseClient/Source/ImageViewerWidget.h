#ifndef IMAGEVIEWERWIDGET_HEADER_
#define IMAGEVIEWERWIDGET_HEADER_

#include <QFrame>
#include <QLabel>
#include <QPixmap>

class ImageViewerWidget : public QFrame {
private:
	Q_OBJECT

public:
	explicit ImageViewerWidget(QWidget* parent = 0);
	~ImageViewerWidget();

	void setPixmap(const QPixmap&);
	QPixmap pixmap() const;

	void setAspectRatio(float);

protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	QLabel* label_;
	QPixmap pixmap_;

	float aspectRatio_;
};

#endif // IMAGEVIEWERWIDGET_HEADER_

