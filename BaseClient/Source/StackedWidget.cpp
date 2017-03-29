#include "StackedWidget.h"

#include <QResizeEvent>
#include <QBoxLayout>

#include <math.h>

StackedWidget::StackedWidget(QWidget* parent) : QStackedWidget(parent)
{
	setObjectName("ImageVideo");

	aspectRatio_ = 0.0f;
}

StackedWidget::~StackedWidget()
{
}

void StackedWidget::setAspectRatio(float aspectRatio)
{
	aspectRatio_ = aspectRatio;
}

void StackedWidget::resizeEvent(QResizeEvent* e)
{
	QSize size = e->size();

	if (aspectRatio_ != 0.0f)
	{
		int height = (int)floor(size.width() / aspectRatio_ + 0.5f);

		if (height != size.height())
		{
			setFixedHeight(height);
		}
	}
}

