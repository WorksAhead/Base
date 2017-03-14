#ifndef PAGESEDITDIALOG_HEADER_
#define PAGESEDITDIALOG_HEADER_

#include "ui_PagesEditDialog.h"

class PagesEditDialog : public QDialog {
private:
	Q_OBJECT

public:
	PagesEditDialog(const QString& text, QWidget* parent = 0);
	~PagesEditDialog();

	QString text();

private Q_SLOTS:
	void onSubmit();

private:
	Ui::PagesEditDialog ui_;
};

#endif // PAGESEDITDIALOG_HEADER_

