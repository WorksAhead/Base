#include "BaseClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BaseClient w;
	w.show();
	return a.exec();
}
