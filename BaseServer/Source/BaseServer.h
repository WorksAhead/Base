#ifndef BASESERVER_BASESERVER_HEADER_
#define BASESERVER_BASESERVER_HEADER_

#include <Ice/Ice.h>

class BaseServer : public Ice::Service {
public:
	virtual bool start(int argc, char* argv[], int& status);
	virtual bool stop();

private:
	Ice::ObjectAdapterPtr adapter_;
};

#endif // BASESERVER_BASESERVER_HEADER_

