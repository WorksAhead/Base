#ifndef LOWERPANEWIDGET_HEADER
#define LOWERPANEWIDGET_HEADER

#include "ui_LowerPaneWidget.h"

class LowerPaneWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit LowerPaneWidget(QWidget* parent = 0);
	~LowerPaneWidget();

protected:
	virtual void paintEvent(QPaintEvent*);

private:
	Ui::LowerPaneWidget ui_;
};

#endif // LOWERPANEWIDGET_HEADER

