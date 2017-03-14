#include "PagesEditDialog.h"

PagesEditDialog::PagesEditDialog(const QString& text, QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.plainTextEdit->setPlainText(text);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &PagesEditDialog::onSubmit);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

PagesEditDialog::~PagesEditDialog()
{

}

QString PagesEditDialog::text()
{
	return ui_.plainTextEdit->toPlainText();
}

void PagesEditDialog::onSubmit()
{
	QDialog::accept();
}

