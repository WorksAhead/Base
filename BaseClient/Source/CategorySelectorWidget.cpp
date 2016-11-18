#include "CategorySelectorWidget.h"
#include "FlowLayout.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVariant>
#include <QMouseEvent>

CategorySelectorWidget::CategorySelectorWidget(QWidget* parent) : QWidget(parent)
{
	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setMargin(0);
	layout_->setSpacing(6);

	setLayout(layout_);
}

CategorySelectorWidget::~CategorySelectorWidget()
{
}

void CategorySelectorWidget::setCategories(const QStringList& list)
{
	FlowLayout* group = 0;

	for (const QString& s : list)
	{
		if (s.isEmpty()) {
			group = 0;
		}
		else if (s.startsWith('=')) {
			QGroupBox* group = new QGroupBox;
			group->setTitle("");
			continue;
		}
		else {
			if (group == 0) {
				group = new FlowLayout(0, 0, 0);
				layout_->addLayout(group);
			}
			QLabel* label = new QLabel;
			label->setObjectName("Category");
			label->setText(s);
			label->setProperty("selected", QVariant(false));
			group->addWidget(label);
		}
	}
}

void CategorySelectorWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		QWidget* w = childAt(e->pos());
		QLabel* label = qobject_cast<QLabel*>(w);
		if (label) {
			bool selected = !label->property("selected").toBool();
			style()->unpolish(label);
			label->setProperty("selected", selected);
			style()->polish(label);
		}
	}
}

