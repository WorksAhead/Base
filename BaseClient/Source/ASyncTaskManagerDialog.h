#ifndef ASYNCTASKMANAGERDIALOG_HEADER_
#define ASYNCTASKMANAGERDIALOG_HEADER_

#include "ui_ASyncTaskManagerDialog.h"

class ASyncTaskManagerDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit ASyncTaskManagerDialog(QWidget* parent = 0);
	~ASyncTaskManagerDialog();

	ASyncTaskListWidget* listWidget() const;

private Q_SLOTS:
	void onCancel();

private:
	Ui::ASyncTaskManagerDialog ui_;
};

#endif // ASYNCTASKMANAGERDIALOG_HEADER_

