#ifndef CATEGORYFILTERWIDGET_HEADER_
#define CATEGORYFILTERWIDGET_HEADER_

#include "LabelSelectorWidget.h"

#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

class CategoryFilterWidget : public QWidget {
private:
	Q_OBJECT

public:
	CategoryFilterWidget(QWidget* parent = 0);
	~CategoryFilterWidget();

	LabelSelectorWidget* labelSelectorWidget();

Q_SIGNALS:
	void collapsed();
	void extended();

public Q_SLOTS:
	void collapse();
	void extend();

private:
	QPushButton* collapseButton_;
	QScrollArea* area_;
	LabelSelectorWidget* selector_;
};

#endif // CATEGORYFILTERWIDGET_HEADER_

