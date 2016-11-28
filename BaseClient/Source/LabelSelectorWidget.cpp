#include "LabelSelectorWidget.h"
#include "FlowLayout.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVariant>
#include <QMouseEvent>

LabelSelectorWidget::LabelSelectorWidget(QWidget* parent) : QWidget(parent)
{
	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setMargin(0);
	layout_->setSpacing(6);

	setLayout(layout_);
}

LabelSelectorWidget::~LabelSelectorWidget()
{
}

void LabelSelectorWidget::setLabels(const QStringList& list)
{
	list_.clear();

	QGroupBox* group = 0;

	for (QString s : list)
	{
		s = s.trimmed();

		if (s.contains('='))
		{
			s.remove('=');
			group = new QGroupBox;
			group->setObjectName("LabelGroup");
			group->setTitle(s);
			group->setLayout(new FlowLayout(0, 0, 0));
			layout_->addWidget(group);
			continue;
		}
		else if (group && !s.isEmpty())
		{
			FlowLayout* layout = qobject_cast<FlowLayout*>(group->layout());
			QLabel* label = new QLabel;
			label->setObjectName("Label");
			label->setText(s);
			label->setProperty("selected", QVariant(false));
			layout->addWidget(label);
			list_ << s;
		}
	}
}

void LabelSelectorWidget::setSelectedLabels(const QStringList& list)
{
	QSet<QString> set;
	for (QString s : list) {
		s = s.trimmed();
		set.insert(s);
	}

	for (int i = 0; i < layout_->count(); ++i)
	{
		QGroupBox* group = qobject_cast<QGroupBox*>(layout_->itemAt(i)->widget());
		FlowLayout* layout = qobject_cast<FlowLayout*>(group->layout());

		for (int j = 0; j < layout->count(); ++j)
		{
			QLabel* label = qobject_cast<QLabel*>(layout->itemAt(j)->widget());

			if (set.contains(label->text())) {
				set_.insert(label->text());
				style()->unpolish(label);
				label->setProperty("selected", true);
				style()->polish(label);
			}
			else {
				set_.remove(label->text());
				style()->unpolish(label);
				label->setProperty("selected", false);
				style()->polish(label);
			}
		}
	}
}

QStringList LabelSelectorWidget::getSelectedLabels()
{
	QStringList list;

	for (const QString& s : list_) {
		if (set_.contains(s)) {
			list << s;
		}
	}

	return list;
}

void LabelSelectorWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QWidget* w = childAt(e->pos());
		QLabel* label = qobject_cast<QLabel*>(w);

		if (label)
		{
			bool selected = !label->property("selected").toBool();

			if (selected) {
				set_.insert(label->text());
			}
			else {
				set_.remove(label->text());
			}

			Q_EMIT clicked(label->text(), selected);

			style()->unpolish(label);
			label->setProperty("selected", selected);
			style()->polish(label);
		}
	}
}

