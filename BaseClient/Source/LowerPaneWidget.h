#ifndef LOWERPANEWIDGET_HEADER
#define LOWERPANEWIDGET_HEADER

#include "Context.h"
#include "ASyncTask.h"

#include "ui_LowerPaneWidget.h"

#include <QTimer>

class LowerPaneWidget : public QWidget {
private:
	Q_OBJECT

public:
	LowerPaneWidget(ContextPtr context, QWidget* parent = 0);
	~LowerPaneWidget();

	void addTask(ASyncTaskPtr);

public Q_SLOTS:
	void clear();

protected Q_SLOTS:
	void onTurn();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void paintEvent(QPaintEvent*);

private:
	ContextPtr context_;
	Ui::LowerPaneWidget ui_;
	QTimer* turnTimer_;
	QTimer* refreshTimer_;
};

#endif // LOWERPANEWIDGET_HEADER

