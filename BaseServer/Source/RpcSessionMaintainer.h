#ifndef BASESERVER_RPCSESSIONMAINTAINER_HEADER_
#define BASESERVER_RPCSESSIONMAINTAINER_HEADER_

#include "RpcSessionImpl.h"

#include <RpcSession.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include <list>
#include <utility>

class RpcSessionMaintainer : public IceUtil::TimerTask, public IceUtil::Monitor<IceUtil::Mutex> {
public:
	typedef std::pair<Rpc::SessionPrx, RpcSessionImplPtr> Session;

public:
	RpcSessionMaintainer();
	~RpcSessionMaintainer();

	void add(const Session&);

protected:
	 virtual void runTimerTask();

private:
	std::list<Session> sessions_;
};

typedef IceUtil::Handle<RpcSessionMaintainer> RpcSessionMaintainerPtr;

#endif // BASESERVER_RPCSESSIONMAINTAINER_HEADER_

