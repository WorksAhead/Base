#ifndef VTABBARWIDGET_HEADER_
#define VTABBARWIDGET_HEADER_

#include <QWidget>
#include <QBoxLayout>

// forward declaration
class VTabBarTabWidget;

class VTabBarWidget : public QWidget {
private:
	Q_OBJECT

public:
	VTabBarWidget(QWidget* parent = 0);
	~VTabBarWidget();

	void addTab(const QString& label);
	void insertTab(int index, const QString& label);

	void addNotification(int);

	void setCurrentIndex(int);

	QFont labelFont() const;
	void setLabelFont(const QFont& font);

Q_SIGNALS:
	void currentIndexChanged(int);

protected:
	virtual void paintEvent(QPaintEvent*);

	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);

private:
	VTabBarTabWidget* tabAt(const QPoint& pos);

private:
	QBoxLayout* layout_;
	QFont labelFont_;
};


#endif // VTABBARWIDGET_HEADER_

