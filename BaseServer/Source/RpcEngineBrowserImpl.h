#ifndef BASESERVER_RPCENGINEBROWSERIMPL_HEADER_
#define BASESERVER_RPCENGINEBROWSERIMPL_HEADER_

#include "Center.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcEngineBrowserImpl : public Rpc::EngineBrowser {
public:
	explicit RpcEngineBrowserImpl(CenterPtr);
	~RpcEngineBrowserImpl();

	Rpc::ErrorCode init();

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::EngineItemSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode finish(const Ice::Current&);

private:
	CenterPtr center_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcEngineBrowserImpl> RpcEngineBrowserImplPtr;

#endif // BASESERVER_RPCENGINEBROWSERIMPL_HEADER_

