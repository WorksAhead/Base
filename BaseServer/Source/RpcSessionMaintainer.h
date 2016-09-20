#ifndef BASESERVER_RPCSESSIONMAINTAINER_HEADER_
#define BASESERVER_RPCSESSIONMAINTAINER_HEADER_

#include "RpcSessionImpl.h"
#include "RpcClientDownloaderImpl.h"

#include <RpcSession.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include <list>
#include <utility>

class RpcSessionMaintainer : public IceUtil::TimerTask, private IceUtil::Monitor<IceUtil::Mutex> {
public:
	typedef std::pair<Rpc::SessionPrx, RpcSessionImplPtr> Session;
	typedef std::pair<Rpc::DownloaderPrx, RpcClientDownloaderImplPtr> ClientDownloader;

public:
	RpcSessionMaintainer();
	~RpcSessionMaintainer();

	void add(const Session&);
	void add(const ClientDownloader&);

protected:
	 virtual void runTimerTask();

private:
	void maintainSessions();
	void maintainClientDownloaders();

private:
	std::list<Session> sessions_;
	std::list<ClientDownloader> clientDownloaders_;
};

typedef IceUtil::Handle<RpcSessionMaintainer> RpcSessionMaintainerPtr;

#endif // BASESERVER_RPCSESSIONMAINTAINER_HEADER_

