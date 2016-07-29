#include "ASyncTaskListWidget.h"
#include "ASyncTaskWidget.h"

#include <QPainter>
#include <QStyleOption>
#include <QGridLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QList>

#include "QtUtils.h"

ASyncTaskListWidget::ASyncTaskListWidget(QWidget* parent) : QWidget(parent)
{
	timer_ = new QTimer(this);
	timer_->setInterval(1000);
	timer_->start();

	listLayout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	listLayout_->setAlignment(Qt::AlignTop);
	listLayout_->setMargin(0);
	listLayout_->setSpacing(0);

	listWidget_ = new QWidget;
	listWidget_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	listWidget_->setLayout(listLayout_);

	scrollArea_ = new QScrollArea;
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea_->setWidgetResizable(true);
	scrollArea_->setWidget(listWidget_);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(scrollArea_);

	setLayout(layout);
}

ASyncTaskListWidget::~ASyncTaskListWidget()
{
}

void ASyncTaskListWidget::addTask(ASyncTaskPtr task)
{
	ASyncTaskWidget* w = new ASyncTaskWidget(task);
	listLayout_->insertWidget(0, w);
	QObject::connect(timer_, &QTimer::timeout, w, &ASyncTaskWidget::refresh);
	w->refresh();
}

ASyncTaskPtr ASyncTaskListWidget::selectedTask() const
{
	return selectedTask_;
}

int ASyncTaskListWidget::clear()
{
	QList<ASyncTaskWidget*> widgetsToBeRemoved;

	for (int i = 0; i < listLayout_->count(); ++i) {
		ASyncTaskWidget* taskWidget = qobject_cast<ASyncTaskWidget*>(listLayout_->itemAt(i)->widget());
		Q_ASSERT(taskWidget);
		int state = taskWidget->task()->state();
		if (state == ASyncTask::state_finished || state == ASyncTask::state_cancelled || state == ASyncTask::state_failed) {
			widgetsToBeRemoved.append(taskWidget);
		}
	}

	while (!widgetsToBeRemoved.empty()) {
		ASyncTaskWidget* taskWidget = widgetsToBeRemoved.front();
		listLayout_->removeWidget(taskWidget);
		if (selectedTask_ == taskWidget->task()) {
			selectedTask_.reset();
		}
		taskWidget->deleteLater();
		widgetsToBeRemoved.pop_front();
	}

	return listLayout_->count();
}

void ASyncTaskListWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		QPoint pos = listWidget_->mapFrom(this, e->pos());
		QWidget* widget = listWidget_->childAt(pos);
		if (widget) {
			ASyncTaskWidget* selectedTaskWidget = qobject_cast<ASyncTaskWidget*>(widget);
			if (!selectedTaskWidget) {
				selectedTaskWidget = findParent<ASyncTaskWidget*>(widget);
			}
			if (selectedTaskWidget) {
				int index = listLayout_->indexOf(selectedTaskWidget);
				selectTaskByIndex(index);
			}
		}
	}
}

//void ASyncTaskListWidget::keyPressEvent(QKeyEvent* e)
//{
//	if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
//	{
//		if (!selectedTask_) {
//			return;
//		}
//
//		int index = -1;
//		for (int i = 0; i < listLayout_->count(); ++i) {
//			ASyncTaskWidget* taskWidget = qobject_cast<ASyncTaskWidget*>(listLayout_->itemAt(i)->widget());
//			Q_ASSERT(taskWidget);
//			if (selectedTask_ == taskWidget->task()) {
//				index = i;
//				break;
//			}
//		}
//
//		if (e->key() == Qt::Key_Up && index > 0) {
//			selectTaskByIndex(index - 1);
//		}
//		else if (e->key() == Qt::Key_Down && index < listLayout_->count() - 1) {
//			selectTaskByIndex(index + 1);
//		}
//	}
//}

void ASyncTaskListWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ASyncTaskListWidget::selectTaskByIndex(int index)
{
	for (int i = 0; i < listLayout_->count(); ++i)
	{
		ASyncTaskWidget* taskWidget = qobject_cast<ASyncTaskWidget*>(listLayout_->itemAt(i)->widget());
		Q_ASSERT(taskWidget);
		if (i == index) {
			taskWidget->setProperty("selected", true);
			selectedTask_ = taskWidget->task();
		}
		else {
			taskWidget->setProperty("selected", false);
		}
		taskWidget->style()->unpolish(taskWidget);
		taskWidget->style()->polish(taskWidget);
	}
}

