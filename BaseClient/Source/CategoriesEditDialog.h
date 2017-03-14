#ifndef CATEGORIESEDITDIALOG_HEADER_
#define CATEGORIESEDITDIALOG_HEADER_

#include "ui_CategoriesEditDialog.h"

class CategoriesEditDialog : public QDialog {
private:
	Q_OBJECT

public:
	CategoriesEditDialog(const QString& text, QWidget* parent = 0);
	~CategoriesEditDialog();

	QString text();

private Q_SLOTS:
	void onSubmit();

private:
	Ui::CategoriesEditDialog ui_;
};

#endif // CATEGORIESEDITDIALOG_HEADER_

