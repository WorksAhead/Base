#include "ASyncTaskWidget.h"

#include <QPainter>
#include <QStyleOption>

ASyncTaskWidget::ASyncTaskWidget(ASyncTaskPtr task, QWidget* parent) : QWidget(parent), task_(task)
{
	ui_.setupUi(this);
}

ASyncTaskWidget::~ASyncTaskWidget()
{
}

ASyncTaskPtr ASyncTaskWidget::task() const
{
	return task_;
}

void ASyncTaskWidget::refresh()
{
	QFontMetrics metrics(ui_.infoLabel->font());
	QString elidedText = metrics.elidedText(task_->information().c_str(), Qt::ElideRight, ui_.infoLabel->width());

	ui_.infoLabel->setText(elidedText);
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

