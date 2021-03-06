#ifndef ASYNCTASKLISTWIDGET_HEADER_
#define ASYNCTASKLISTWIDGET_HEADER_

#include "ASyncTask.h"

#include <QWidget>
#include <QBoxLayout>
#include <QScrollArea>
#include <QTimer>

class ASyncTaskListWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ASyncTaskListWidget(QWidget* parent = 0);
	~ASyncTaskListWidget();

	int clear();

	void addTask(ASyncTaskPtr);

	ASyncTaskPtr selectedTask() const;

protected:
	virtual void mousePressEvent(QMouseEvent*);

	//virtual void keyPressEvent(QKeyEvent*);

	virtual void paintEvent(QPaintEvent*);

private:
	void selectTaskByIndex(int);

private:
	QTimer* timer_;
	QWidget* listWidget_;
	QBoxLayout* listLayout_;
	QScrollArea* scrollArea_;
	ASyncTaskPtr selectedTask_;
};

#endif // ASYNCTASKLISTWIDGET_HEADER_

