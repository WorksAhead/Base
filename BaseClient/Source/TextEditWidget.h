#ifndef TEXTEDITWIDGET_HEADER_
#define TEXTEDITWIDGET_HEADER_

#include "ui_TextEditWidget.h"

class TextEditWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit TextEditWidget(QWidget* parent = 0);
	~TextEditWidget();

	void clear();

	QString toPlainText();
	QString toHtml();

private Q_SLOTS :
	void onBold();
	void onItalic();
	void onUnderline();
	void onEmoji();

private:
	Ui::TextEditWidget ui_;
};

#endif // TEXTEDITWIDGET_HEADER_

