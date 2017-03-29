#ifndef BASESERVER_RPCCOMMENTBROWSERIMPL_HEADER_
#define BASESERVER_RPCCOMMENTBROWSERIMPL_HEADER_

#include "Context.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcCommentBrowserImpl : public Rpc::CommentBrowser {
public:
	explicit RpcCommentBrowserImpl(ContextPtr);
	~RpcCommentBrowserImpl();

	Rpc::ErrorCode init(const std::string&, const std::string&);

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::CommentSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	ContextPtr context_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcCommentBrowserImpl> RpcCommentBrowserImplPtr;

#endif // BASESERVER_RPCCOMMENTBROWSERIMPL_HEADER_

