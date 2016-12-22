#include "AutoExpandTextEdit.h"

#include <QTextBlock>

#include <algorithm>
#include <math.h>

#define MIN_LINE_COUNT 5

AutoExpandTextEdit::AutoExpandTextEdit(QWidget* parent) : QPlainTextEdit(parent)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QObject::connect(this, &QPlainTextEdit::textChanged, this, &AutoExpandTextEdit::onTextChanged);

	lineHeight_ = (int)floor(document()->begin().layout()->lineAt(0).height() + 0.5);

	setFixedHeight(lineHeight_ * MIN_LINE_COUNT);
}

AutoExpandTextEdit::~AutoExpandTextEdit()
{

}

void AutoExpandTextEdit::onTextChanged()
{
	int n = std::max(MIN_LINE_COUNT, document()->lineCount() + 1);
	qreal h = lineHeight_ * n;

	if (height() != h) {
		setFixedHeight(h);
	}
}

