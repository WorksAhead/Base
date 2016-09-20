#ifndef SUBMITCLIENTDIALOG_HEADER_
#define SUBMITCLIENTDIALOG_HEADER_

#include "Context.h"

#include "ui_SubmitClientDialog.h"

#include <QString>

class SubmitClientDialog : public QDialog {
private:
	Q_OBJECT

public:
	SubmitClientDialog(ContextPtr, QWidget* parent = 0);
	~SubmitClientDialog();

	void switchToEditMode();

	void setVersion(const QString& version);
	void setInfo(const QString& info);

	QString getLocation() const;
	QString getVersion() const;
	QString getInfo() const;

private Q_SLOTS:
	void selectLocation();
	void submit();

private:
	ContextPtr context_;
	Ui::SubmitClientDialog ui_;
	bool editMode_;
};

#endif // SUBMITCLIENTDIALOG_HEADER_

