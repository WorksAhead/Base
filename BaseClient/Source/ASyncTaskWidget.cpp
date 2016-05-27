#include "ASyncTaskWidget.h"

#include <QPainter>
#include <QStyleOption>

ASyncTaskWidget::ASyncTaskWidget(ASyncTask* task, QWidget* parent) : QWidget(parent), task_(task)
{
	ui_.setupUi(this);
}

ASyncTaskWidget::~ASyncTaskWidget()
{
}

ASyncTask* ASyncTaskWidget::task() const
{
	return task_.data();
}

void ASyncTaskWidget::refresh()
{
	ui_.infoLabel->setText(task_->information().c_str());
	ui_.progressBar->setValue(task_->progress());

	switch (task_->state())
	{
	case ASyncTask::state_idle:
		ui_.stateLabel->setText(tr("Idle"));
		break;
	case ASyncTask::state_running:
		ui_.stateLabel->setText(tr("Running"));
		break;
	case ASyncTask::state_finished:
		ui_.stateLabel->setText(tr("Finished"));
		break;
	case ASyncTask::state_failed:
		ui_.stateLabel->setText(tr("Failed"));
		break;
	case ASyncTask::state_cancelled:
		ui_.stateLabel->setText(tr("Cancelled"));
		break;
	}
}

void ASyncTaskWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

