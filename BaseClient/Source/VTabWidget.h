#ifndef VTABWIDGET_HEADER_
#define VTABWIDGET_HEADER_

#include <QWidget>
#include <QListWidget>
#include <QStackedLayout>

class VTabWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit VTabWidget(QWidget* parent = 0);
	~VTabWidget();

	void addTab(const QString& label, QWidget* content);
	void insertTab(int index, const QString& label, QWidget* content);

	void setCurrentIndex(int);

private:
	QListWidget* list_;
	QStackedLayout* stack_;
};

#endif // VTABWIDGET_HEADER_

