#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include "Window.h"

#include <QTabWidget>

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	BaseClient();
	~BaseClient();

private:
	QTabWidget* tab_;
};

#endif // BASECLIENT_HEADER_

