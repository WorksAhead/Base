#include "CategorySelectorDialog.h"

CategorySelectorDialog::CategorySelectorDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui_.setupUi(this);
}

CategorySelectorDialog::~CategorySelectorDialog()
{
}

CategorySelectorWidget* CategorySelectorDialog::categorySelectorWidget()
{
	return ui_.widget;
}

