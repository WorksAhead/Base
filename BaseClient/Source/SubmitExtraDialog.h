#ifndef SUBMITEXTRADIALOG_HEADER_
#define SUBMITEXTRADIALOG_HEADER_

#include "ui_SubmitExtraDialog.h"

#include <QString>

class SubmitExtraDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit SubmitExtraDialog(QWidget* parent = 0);
	~SubmitExtraDialog();

	void switchToEditMode();

	void setTitle(const QString& title);
	void setSetup(const QString& setup);
	void setInfo(const QString& info);

	QString getTitle() const;
	QString getLocation() const;
	QString getSetup() const;
	QString getInfo() const;

private Q_SLOTS:
	void selectLocation();
	void submit();

private:
	Ui::SubmitExtraDialog ui_;
	bool editMode_;
};

#endif // SUBMITEXTRADIALOG_HEADER_

