#include "BaseClient.h"

#include <QTextEdit>

BaseClient::BaseClient()
{
	setWindowTitle("Base");
	setCentralWidget(new QTextEdit);
}

BaseClient::~BaseClient()
{
}
