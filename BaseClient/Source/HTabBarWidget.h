#ifndef HTABBARWIDGET_HEADER_
#define HTABBARWIDGET_HEADER_

#include <QWidget>
#include <QBoxLayout>

// forward declaration
class HTabBarTabWidget;

class HTabBarWidget : public QWidget {
private:
	Q_OBJECT

public:
	HTabBarWidget(QWidget* parent = 0);
	~HTabBarWidget();

	void addTab(const QString& label);
	void insertTab(int index, const QString& label);

	void addNotification(int);

	void setCurrentIndex(int);

	QFont labelFont() const;
	void setLabelFont(const QFont& font);

Q_SIGNALS:
	void currentIndexChanged(int);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);

private:
	HTabBarTabWidget* tabAt(const QPoint& pos);

private:
	QBoxLayout* layout_;
	QFont labelFont_;
};


#endif // HTABBARWIDGET_HEADER_

