#include "CategoriesEditDialog.h"

#include <QTextStream>
#include <QRegularExpression>

CategoriesEditDialog::CategoriesEditDialog(const QString& text, QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.plainTextEdit->setPlainText(text);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &CategoriesEditDialog::onSubmit);
	QObject::connect(ui_.cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

CategoriesEditDialog::~CategoriesEditDialog()
{

}

QString CategoriesEditDialog::text()
{
	return ui_.plainTextEdit->toPlainText();
}

void CategoriesEditDialog::onSubmit()
{
	QString text = ui_.plainTextEdit->toPlainText();
	QTextStream stream(&text, QIODevice::ReadOnly);

	QString line;
	qint64 lastPos = 0;

	while (stream.readLineInto(&line))
	{
		int pos = line.indexOf(QRegularExpression("[,()]"));
		if (pos >= 0) {
			pos = lastPos + pos;
			QTextCursor c = ui_.plainTextEdit->textCursor();
			c.setPosition(pos);
			c.setPosition(pos + 1, QTextCursor::KeepAnchor);
			ui_.plainTextEdit->setTextCursor(c);
			ui_.plainTextEdit->setFocus();
			return;
		}

		lastPos = stream.pos();
	}

	QDialog::accept();
}

