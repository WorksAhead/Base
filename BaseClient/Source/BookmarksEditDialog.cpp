#include "BookmarksEditDialog.h"

BookmarksEditDialog::BookmarksEditDialog(const QString& text, QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.plainTextEdit->setPlainText(text);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &BookmarksEditDialog::onSubmit);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

BookmarksEditDialog::~BookmarksEditDialog()
{

}

QString BookmarksEditDialog::text()
{
	return ui_.plainTextEdit->toPlainText();
}

void BookmarksEditDialog::onSubmit()
{
	QDialog::accept();
}

