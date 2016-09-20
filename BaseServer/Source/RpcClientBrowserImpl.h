#ifndef BASESERVER_RPCCLIENTBROWSERIMPL_HEADER_
#define BASESERVER_RPCCLIENTBROWSERIMPL_HEADER_

#include "Center.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcClientBrowserImpl : public Rpc::ClientBrowser {
public:
	explicit RpcClientBrowserImpl(CenterPtr);
	~RpcClientBrowserImpl();

	Rpc::ErrorCode init();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::ClientInfoSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	CenterPtr center_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcClientBrowserImpl> RpcClientBrowserImplPtr;

#endif // BASESERVER_RPCCLIENTBROWSERIMPL_HEADER_

