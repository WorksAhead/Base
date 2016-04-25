#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include "Window.h"

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	BaseClient();
	~BaseClient();
};

#endif // BASECLIENT_HEADER_

