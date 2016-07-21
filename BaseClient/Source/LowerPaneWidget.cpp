#include "LowerPaneWidget.h"

#include <QPainter>

LowerPaneWidget::LowerPaneWidget(QWidget* parent)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);
}

LowerPaneWidget::~LowerPaneWidget()
{
}

void LowerPaneWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
