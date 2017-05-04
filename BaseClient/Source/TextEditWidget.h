#ifndef TEXTEDITWIDGET_HEADER_
#define TEXTEDITWIDGET_HEADER_

#include "ui_TextEditWidget.h"

// forward declaration
class EmojiInputDialog;

class TextEditWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit TextEditWidget(QWidget* parent = 0);
	~TextEditWidget();

	void clear();

	void setPlaceholderText(const QString&);

	void setHtml(const QString&);
	void setPlainText(const QString&);

	QString toPlainText();
	QString toHtml();

private Q_SLOTS :
	void onBold();
	void onItalic();
	void onUnderline();
	void onHyperlink();
	void onEmoji();

private:
	Ui::TextEditWidget ui_;

	EmojiInputDialog* d_;
};

#endif // TEXTEDITWIDGET_HEADER_

