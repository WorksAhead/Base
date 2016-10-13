#ifndef WINDOW_HEADER_
#define WINDOW_HEADER_

#include "EdgeDetector.h"

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QPoint>
#include <QShowEvent>
#include <QEvent>
#include <QList>
#include <QLabel>

// forward declaration
class Title;

class Window : public QWidget {
private:
	Q_OBJECT

public:
	Window();
	~Window();

	void setWindowIcon(const QIcon&);

	void setWindowTitle(const QString&);
	QString windowTitle() const;

	void setExpressWidget(QWidget*, QWidget** oldExpressWidget = 0);
	QWidget* expressWidget() const;

	void setDecoratorWidget(QWidget*, QWidget** outOldDecoratorWidget = 0);
	QWidget* decoratorWidget() const;

	void setCentralWidget(QWidget*, QWidget** outOldCentralWidget = 0);
	QWidget* centralWidget() const;

	QRect frameGeometry() const;
	const QRect& geometry() const;
	QRect frameNormalGeometry() const;
	QRect normalGeometry() const;

	void setFrameGeometry(const QRect&);
	void setGeometry(const QRect&);

	int x() const;
	int y() const;
	QPoint pos() const;

	QSize frameSize() const;
	int frameWidth() const;
	int frameHeight() const;

	QSize size() const;
	int width() const;
	int height() const;

	QRect rect() const;

	QSize minimumSize() const;
	QSize maximumSize() const;
	int minimumWidth() const;
	int minimumHeight() const;
	int maximumWidth() const;
	int maximumHeight() const;

	void setMinimumSize(const QSize&);
	void setMinimumSize(int minw, int minh);
	void setMaximumSize(const QSize&);
	void setMaximumSize(int maxw, int maxh);
	void setMinimumWidth(int minw);
	void setMinimumHeight(int minh);
	void setMaximumWidth(int maxw);
	void setMaximumHeight(int maxh);

	void setFixedSize(const QSize&);
	void setFixedSize(int w, int h);
	void setFixedWidth(int w);
	void setFixedHeight(int h);

	void move(int x, int y);
	void move(const QPoint&);
	void resize(int w, int h);
	void resize(const QSize&);

	QPoint origin() const;

	bool resizeable() const;
	void setResizeable(bool);

	void registerEventReceiver(QObject*, int priority = 0);
	void unregisterEventReceiver(QObject*);

protected:
	virtual bool event(QEvent*);

	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);

	virtual void paintEvent(QPaintEvent*);

private:
	void setFrame(int);
	void onStateChange();
	void onDrag(const QPoint&);
	void onHover(const QPoint&);
	QPushButton* createStateButton(int);

private:
	QLabel* iconWidget_;
	QLabel* titleWidget_;
	QWidget* expressWidget_;
	QWidget* decoratorWidget_;
	QWidget* centralWidget_;

	QWidget* topWidget_;
	QBoxLayout* topLayout_;
	QBoxLayout* layout_;

	QPushButton* minimizeButton_;
	QPushButton* maximizeOrRestoreButton_;
	QPushButton* closeButton_;

	EdgeDetector hoverDetector_;
	EdgeDetector dragDetector_;

	int frame_;

	QPoint dragStartPos_;
	bool cursorChanged_;
	bool dragging_;
};

#endif // WINDOW_HEADER_

