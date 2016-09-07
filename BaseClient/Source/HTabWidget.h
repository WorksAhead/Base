#ifndef HTABWIDGET_HEADER_
#define HTABWIDGET_HEADER_

#include "HTabBarWidget.h"

#include <QWidget>
#include <QStackedLayout>

class HTabWidget : public QWidget {
private:
	Q_OBJECT

public:
	HTabWidget(QWidget* parent = 0);
	~HTabWidget();

	HTabBarWidget* tabBar();

	void addTab(const QString& label, QWidget* content);
	void insertTab(int index, const QString& label, QWidget* content);

	int indexOf(QWidget* content);

	void addNotification(int);

	int currentIndex() const;
	void setCurrentIndex(int);

	QFont labelFont() const;
	void setLabelFont(const QFont&);

private:
	HTabBarWidget* tabBar_;
	QStackedLayout* stack_;
};

#endif // HTABWIDGET_HEADER_

