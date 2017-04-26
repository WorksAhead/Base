#include "TextEditWidget.h"
#include "EmojiInputDialog.h"
#include "Emoji.h"

#include <QApplication>
#include <QDesktopWidget>

TextEditWidget::TextEditWidget(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	d_ = new EmojiInputDialog;

	d_->show();
	d_->layout()->invalidate();
	d_->hide();

	QObject::connect(ui_.boldButton, &QPushButton::clicked, this, &TextEditWidget::onBold);
	QObject::connect(ui_.italicButton, &QPushButton::clicked, this, &TextEditWidget::onItalic);
	QObject::connect(ui_.underlineButton, &QPushButton::clicked, this, &TextEditWidget::onUnderline);
	QObject::connect(ui_.emojiButton, &QPushButton::clicked, this, &TextEditWidget::onEmoji);

	QObject::connect(d_, &EmojiInputDialog::emojiPressed, [this](const QString& image)
	{
		QTextCursor cursor = ui_.textEdit->textCursor();
		cursor.insertImage(image);
	});

	addEmojiResourcesToDocument(ui_.textEdit->document());
}

TextEditWidget::~TextEditWidget()
{
}

void TextEditWidget::clear()
{
	ui_.textEdit->clear();
	addEmojiResourcesToDocument(ui_.textEdit->document());
}

void TextEditWidget::setHtml(const QString& html)
{
	ui_.textEdit->setHtml(html);
}

void TextEditWidget::setPlainText(const QString& text)
{
	ui_.textEdit->setPlainText(text);
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
	QPoint pos = ui_.emojiButton->mapToGlobal(QPoint(ui_.emojiButton->width() / 2, 0));

	QRect rect = d_->rect();

	rect.moveTop(pos.y() - rect.height());
	rect.moveLeft(pos.x() - rect.width() / 2);

	QRect desktopRect = qApp->desktop()->availableGeometry(this);

	if (rect.right() > desktopRect.right()) {
		rect.moveRight(desktopRect.right());
	}

	if (rect.bottom() > desktopRect.bottom()) {
		rect.moveBottom(desktopRect.bottom());
	}

	if (rect.left() < 0) {
		rect.moveLeft(0);
	}

	if (rect.top() < 0) {
		rect.moveTop(0);
	}

	d_->move(rect.topLeft());

	d_->exec();
}

