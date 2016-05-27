#ifndef ASYNCTASKWIDGET_HEADER_
#define ASYNCTASKWIDGET_HEADER_

#include "ASyncTask.h"
#include "ui_ASyncTaskWidget.h"

#include <QWidget>
#include <QSharedPointer>

class ASyncTaskWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ASyncTaskWidget(ASyncTask*, QWidget* parent = 0);
	~ASyncTaskWidget();

	ASyncTask* task() const;

public Q_SLOTS:
	void refresh();

protected:
	void paintEvent(QPaintEvent*);

private:
	Ui::AsyncTaskWidget ui_;
	QSharedPointer<ASyncTask> task_;
};

#endif // ASYNCTASKWIDGET_HEADER_

