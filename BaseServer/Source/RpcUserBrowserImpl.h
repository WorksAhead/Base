#ifndef BASESERVER_RPCUSERBROWSERIMPL_HEADER_
#define BASESERVER_RPCUSERBROWSERIMPL_HEADER_

#include "Context.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcUserBrowserImpl : public Rpc::UserBrowser {
public:
	explicit RpcUserBrowserImpl(ContextPtr);
	~RpcUserBrowserImpl();

	Rpc::ErrorCode init();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::UserSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	ContextPtr context_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcUserBrowserImpl> RpcUserBrowserImplPtr;

#endif // BASESERVER_RPCUSERBROWSERIMPL_HEADER_

