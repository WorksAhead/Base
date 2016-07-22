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
	explicit ASyncTaskWidget(ASyncTaskPtr, QWidget* parent = 0);
	~ASyncTaskWidget();

	ASyncTaskPtr task() const;

public Q_SLOTS:
	void refresh();

protected:
	void paintEvent(QPaintEvent*);

private:
	Ui::AsyncTaskWidget ui_;
	ASyncTaskPtr task_;
};

#endif // ASYNCTASKWIDGET_HEADER_

