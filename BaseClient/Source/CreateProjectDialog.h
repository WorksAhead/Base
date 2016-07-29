#ifndef CREATEPROJECTDIALOG_HEADER_
#define CREATEPROJECTDIALOG_HEADER_

#include <ui_CreateProjectDialog.h>

class CreateProjectDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit CreateProjectDialog(QWidget* parent = 0);
	~CreateProjectDialog();

	void setDirectory(const QString&);

	QString location() const;

private Q_SLOTS:
	void onSelect();
	void onCreate();

private:
	Ui::CreateProjectDialog ui_;
};

#endif // CREATEPROJECTDIALOG_HEADER_

