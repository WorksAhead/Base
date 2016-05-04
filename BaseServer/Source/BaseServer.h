#ifndef BASESERVER_BASESERVER_HEADER_
#define BASESERVER_BASESERVER_HEADER_

#include "SQLiteUtil.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

class BaseServer : public Ice::Service {
public:
	virtual bool start(int argc, char* argv[], int& status);
	virtual bool stop();

private:
	Ice::ObjectAdapterPtr adapter_;
	IceUtil::TimerPtr timer_;
};

#endif // BASESERVER_BASESERVER_HEADER_

