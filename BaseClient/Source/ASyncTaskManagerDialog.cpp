#include "ASyncTaskManagerDialog.h"

ASyncTaskManagerDialog::ASyncTaskManagerDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.clearButton, &QPushButton::clicked, this, &ASyncTaskManagerDialog::onClear);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &ASyncTaskManagerDialog::onCancel);
}

ASyncTaskManagerDialog::~ASyncTaskManagerDialog()
{
}

ASyncTaskListWidget* ASyncTaskManagerDialog::listWidget() const
{
	return ui_.taskList;
}

void ASyncTaskManagerDialog::onClear()
{
	if (ui_.taskList->clear() == 0) {
		Q_EMIT cleared();
	}
}

void ASyncTaskManagerDialog::onCancel()
{
	ASyncTaskPtr task = ui_.taskList->selectedTask();
	if (task) {
		task->cancel();
	}
}

