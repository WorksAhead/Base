#ifndef BASESERVER_RPCENGINEBROWSERIMPL_HEADER_
#define BASESERVER_RPCENGINEBROWSERIMPL_HEADER_

#include "Center.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcEngineVersionBrowserImpl : public Rpc::EngineVersionBrowser {
public:
	explicit RpcEngineVersionBrowserImpl(CenterPtr);
	~RpcEngineVersionBrowserImpl();

	Rpc::ErrorCode init();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::EngineVersionSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	CenterPtr center_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcEngineVersionBrowserImpl> RpcEngineBrowserImplPtr;

#endif // BASESERVER_RPCENGINEBROWSERIMPL_HEADER_

