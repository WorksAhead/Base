#include "Emoji.h"

#include <QImage>

#include <algorithm>
#include <vector>
#include <string>

void addEmojiResourcesToDocument(QTextDocument* doc)
{
	QStringList list = getEmojiList();

	for (const QString& name : list)
	{
		doc->addResource(QTextDocument::ImageResource, QUrl(name), QImage(":/Emoji/" + name));
	}
}

QStringList getEmojiList()
{
	QStringList list;

	for (int i = 1; i <= 189; ++i) {
		list.append(QString::asprintf("Emoji Smiley-%02d.png", i));
	}

	for (int i = 1; i <= 116; ++i) {
		list.append(QString::asprintf("Emoji Natur-%02d.png", i));
	}

	for (int i = 1; i <= 230; ++i) {
		list.append(QString::asprintf("Emoji Objects-%02d.png", i));
	}

	for (int i = 1; i <= 101; ++i) {
		list.append(QString::asprintf("Emoji Orte-%02d.png", i));
	}

	for (int i = 1; i <= 209; ++i) {
		list.append(QString::asprintf("Emoji Symbols-%02d.png", i));
	}

	return list;
}

