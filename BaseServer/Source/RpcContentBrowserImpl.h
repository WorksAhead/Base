#ifndef BASESERVER_RPCCONTENTBROWSERIMPL_HEADER_
#define BASESERVER_RPCCONTENTBROWSERIMPL_HEADER_

#include "Center.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcContentBrowserImpl : public Rpc::ContentBrowser {
public:
	explicit RpcContentBrowserImpl(CenterPtr);
	~RpcContentBrowserImpl();

	Rpc::ErrorCode init(const std::string& page, const std::string& category);

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::ContentItemSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	CenterPtr center_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcContentBrowserImpl> RpcContentBrowserImplPtr;

#endif // BASESERVER_RPCCONTENTBROWSERIMPL_HEADER_

