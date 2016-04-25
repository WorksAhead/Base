#include "BaseClient.h"

#include <QApplication>
#include <QPalette>
#include <QFont>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QPalette pal = a.palette();
	pal.setColor(QPalette::Window, QColor(40, 40, 40));
	pal.setColor(QPalette::Background, QColor(40, 40, 40));
	pal.setColor(QPalette::Base, QColor(30, 30, 30));
	pal.setColor(QPalette::WindowText, QColor(250, 250, 250));
	pal.setColor(QPalette::Text, QColor(250, 250, 250));
	a.setPalette(pal);

	QFont font;
	//font.setFamily(QStringLiteral("Segoe UI"));
	font.setFamily(QStringLiteral("Microsoft YaHei UI"));
	//font.setPointSize(9);
	a.setFont(font);

	BaseClient w;
	w.setMinimumSize(800, 500);
	w.resize(1280, 800);
	w.show();

	return a.exec();
}
