#include "EmojiInputDialog.h"
#include "Emoji.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QGuiApplication>

EmojiInputDialog::EmojiInputDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);

	QWidget* widget = new QWidget;

	QGridLayout* widgetLayout = new QGridLayout;

	const int size = 24;
	const int space = 6;
	const int column = 14;

	widgetLayout->setContentsMargins(space, space, space, space);
	widgetLayout->setSpacing(space);

	QStringList list = getEmojiList();

	for (int i = 0; i < list.count(); ++i)
	{
		QLabel* label = new QLabel;
		label->setPixmap(QPixmap(":/Emoji/" + list[i]));
		label->setProperty("image", list[i]);

		int row = i / column;

		widgetLayout->addWidget(label, row, i % column);

		if (i == list.count() - 1)
		{
			int n = column - (row + 1) % column;

			if (n)
			{
				//widgetLayout->setContentsMargins(space, space, space, (space + size) * n + space);
			}
		}
	}

	widget->setLayout(widgetLayout);

	scrollArea_ = new QScrollArea;

	scrollArea_->setWidget(widget);
	scrollArea_->setFixedHeight(column * size + (column + 1) * space);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	scrollArea_->verticalScrollBar()->setSingleStep(space + size);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);

	layout->setMargin(0);
	layout->addWidget(scrollArea_);

	setLayout(layout);
}

EmojiInputDialog::~EmojiInputDialog()
{
}

void EmojiInputDialog::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QLabel* label = qobject_cast<QLabel*>(childAt(e->pos()));

		if (label)
		{
			Q_EMIT emojiPressed(label->property("image").toString());

			if (QGuiApplication::keyboardModifiers() != Qt::ControlModifier)
			{
				done(0);
			}
		}
		else if (!rect().contains(e->pos()))
		{
			done(0);
		}
	}
}

void EmojiInputDialog::mouseReleaseEvent(QMouseEvent*)
{
}

void EmojiInputDialog::mouseDoubleClickEvent(QMouseEvent*)
{
}

void EmojiInputDialog::showEvent(QShowEvent*)
{
	scrollArea_->verticalScrollBar()->setValue(0);
}

