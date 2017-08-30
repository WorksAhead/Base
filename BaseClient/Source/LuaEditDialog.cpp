#include "LuaEditDialog.h"

#include <QMessageBox>

#include <kaguya.hpp>

LuaEditDialog::LuaEditDialog(const QString& text, QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.plainTextEdit->setPlainText(text);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &LuaEditDialog::onSubmit);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

LuaEditDialog::~LuaEditDialog()
{

}

QString LuaEditDialog::text()
{
	return ui_.plainTextEdit->toPlainText();
}

void LuaEditDialog::onSubmit()
{
	kaguya::State state;

	QString message;

	state.setErrorHandler([&message](int, const char* msg)
	{
		message = msg;
	});

	if (!state.dostring(ui_.plainTextEdit->toPlainText().toStdString()))
	{
		QMessageBox::information(this, "Base", message);
		return;
	}

	QDialog::accept();
}

