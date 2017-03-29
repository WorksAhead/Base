#include "TextEditWidget.h"

TextEditWidget::TextEditWidget(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	QObject::connect(ui_.boldButton, &QPushButton::clicked, this, &TextEditWidget::onBold);
	QObject::connect(ui_.italicButton, &QPushButton::clicked, this, &TextEditWidget::onItalic);
	QObject::connect(ui_.underlineButton, &QPushButton::clicked, this, &TextEditWidget::onUnderline);
	QObject::connect(ui_.emojiButton, &QPushButton::clicked, this, &TextEditWidget::onEmoji);
}

TextEditWidget::~TextEditWidget()
{
}

void TextEditWidget::clear()
{
	ui_.textEdit->clear();
}

QString TextEditWidget::toPlainText()
{
	return ui_.textEdit->toPlainText();
}

QString TextEditWidget::toHtml()
{
	return ui_.textEdit->toHtml();
}

void TextEditWidget::onBold()
{
	QTextCursor cursor = ui_.textEdit->textCursor();

	if (cursor.hasSelection())
	{
		QTextCharFormat f = cursor.charFormat();

		if (f.fontWeight() == QFont::Normal) {
			f.setFontWeight(QFont::Bold);
		}
		else {
			f.setFontWeight(QFont::Normal);
		}

		cursor.mergeCharFormat(f);
	}
}

void TextEditWidget::onItalic()
{
	QTextCursor cursor = ui_.textEdit->textCursor();

	if (cursor.hasSelection())
	{
		QTextCharFormat f = cursor.charFormat();

		f.setFontItalic(!f.fontItalic());

		cursor.mergeCharFormat(f);
	}
}

void TextEditWidget::onUnderline()
{
	QTextCursor cursor = ui_.textEdit->textCursor();

	if (cursor.hasSelection())
	{
		QTextCharFormat f = cursor.charFormat();

		f.setFontUnderline(!f.fontUnderline());

		cursor.mergeCharFormat(f);
	}
}

void TextEditWidget::onEmoji()
{
	ui_.textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("SlightlySmilingFace"), QImage(":/Icons/SlightlySmilingFace.png"));

	QTextCursor cursor = ui_.textEdit->textCursor();

	cursor.insertImage("SlightlySmilingFace");
}

