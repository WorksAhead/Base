#ifndef CROPPINGBOXWIDGET_HEADER
#define CROPPINGBOXWIDGET_HEADER

#include "EdgeDetector.h"

#include <QWidget>

class CroppingBoxWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit CroppingBoxWidget(QWidget* parent);
	~CroppingBoxWidget();

public Q_SLOTS:
	void setRatio(const QSize& size);
	void setArea(const QRect& area);

protected:
	virtual bool event(QEvent*);

	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);

	virtual void paintEvent(QPaintEvent*);

private:
	void onDrag(const QPoint&);
	void onHover(const QPoint&);

private:
	double ratio_;
	QRect area_;

	EdgeDetector hoverDetector_;
	EdgeDetector dragDetector_;

	QPoint dragStartPos_;
	bool cursorChanged_;
	bool dragging_;
};

#endif // CROPPINGBOXWIDGET_HEADER

