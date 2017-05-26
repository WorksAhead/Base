#include "BaseServer.h"
#include "RpcStartImpl.h"
#include "Center.h"

#include <IceUtil/IceUtil.h>

bool BaseServer::start(int argc, char* argv[], int& status)
{
	try {
		CenterPtr center(new Center);

		IceUtil::TimerPtr timer = new IceUtil::Timer();
		timer->scheduleRepeated(center, IceUtil::Time::seconds(5));

		adapter_ = communicator()->createObjectAdapter("BaseServer");
		adapter_->add(new RpcStartImpl(center), communicator()->stringToIdentity("Start"));
		adapter_->activate();
	}
	catch (Ice::LocalException&) {
		status = EXIT_FAILURE;
		throw;
	}

	status = EXIT_SUCCESS;
	return true;
}

bool BaseServer::stop()
{
	return true;
}

