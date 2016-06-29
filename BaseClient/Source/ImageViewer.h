#ifndef IMAGEVIEWER_HEADER_
#define IMAGEVIEWER_HEADER_

#include <QFrame>
#include <QLabel>
#include <QPixmap>

class ImageViewer : public QFrame {
private:
	Q_OBJECT

public:
	explicit ImageViewer(QWidget* parent = 0);
	~ImageViewer();

	void setPixmap(const QPixmap&);
	QPixmap pixmap() const;

protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	QLabel* label_;
	QPixmap pixmap_;
};

#endif // IMAGEVIEWER_HEADER_

