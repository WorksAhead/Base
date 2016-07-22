#include "ASyncTaskManagerDialog.h"

ASyncTaskManagerDialog::ASyncTaskManagerDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.clearButton, &QPushButton::clicked, ui_.taskList, &ASyncTaskListWidget::clear);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &ASyncTaskManagerDialog::onCancel);
}

ASyncTaskManagerDialog::~ASyncTaskManagerDialog()
{
}

ASyncTaskListWidget* ASyncTaskManagerDialog::listWidget() const
{
	return ui_.taskList;
}

void ASyncTaskManagerDialog::onCancel()
{
	ASyncTaskPtr task = ui_.taskList->selectedTask();
	if (task) {
		task->cancel();
	}
}

