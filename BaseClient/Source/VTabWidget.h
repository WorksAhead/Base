#ifndef VTABWIDGET_HEADER_
#define VTABWIDGET_HEADER_

#include "VTabBarWidget.h"

#include <QWidget>
#include <QStackedLayout>

class VTabWidget : public QWidget {
private:
	Q_OBJECT

public:
	VTabWidget(QWidget* parent = 0);
	~VTabWidget();

	VTabBarWidget* tabBar();

	void addTab(const QString& label, QWidget* content);
	void insertTab(int index, const QString& label, QWidget* content);

	int indexOf(QWidget* content);

	void addNotification(int);

	int currentIndex() const;
	void setCurrentIndex(int);

	QFont labelFont() const;
	void setLabelFont(const QFont&);

private:
	VTabBarWidget* tabBar_;
	QStackedLayout* stack_;
};

#endif // VTABWIDGET_HEADER_

