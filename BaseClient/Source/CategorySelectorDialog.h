#ifndef CATEGORYSELECTORDIALOG_HEADER_
#define CATEGORYSELECTORDIALOG_HEADER_

#include <ui_CategorySelectorDialog.h>

#include <QDialog>

class CategorySelectorDialog : public QDialog {
private:
	Q_OBJECT

public:
	CategorySelectorDialog(QWidget* parent = 0);
	~CategorySelectorDialog();

	CategorySelectorWidget* categorySelectorWidget();

private:
	Ui::CategorySelectorDialog ui_;
};

#endif // CATEGORYSELECTORDIALOG_HEADER_

