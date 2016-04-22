#ifndef BASECLIENT_H
#define BASECLIENT_H

#include <QtWidgets/QWidget>
#include "ui_BaseClient.h"

class BaseClient : public QWidget
{
	Q_OBJECT

public:
	BaseClient(QWidget *parent = 0);
	~BaseClient();

private:
	Ui::BaseClientClass ui;
};

#endif // BASECLIENT_H
