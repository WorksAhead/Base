#ifndef BASESERVER_RPCEXTRABROWSERIMPL_HEADER_
#define BASESERVER_RPCEXTRABROWSERIMPL_HEADER_

#include "Center.h"

#include <SQLiteCpp/Statement.h>

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

class RpcExtraBrowserImpl : public Rpc::ExtraBrowser {
public:
	explicit RpcExtraBrowserImpl(CenterPtr);
	~RpcExtraBrowserImpl();

	Rpc::ErrorCode init(const std::string& category, const std::string& search);
	Rpc::ErrorCode init(const std::string& parentId);

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode next(Ice::Int, Rpc::ExtraInfoSeq&, const Ice::Current&);

private:
	void checkIsDestroyed();

private:
	CenterPtr center_;
	bool destroyed_;
	boost::shared_ptr<SQLite::Statement> s_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcExtraBrowserImpl> RpcExtraBrowserImplPtr;

#endif // BASESERVER_RPCEXTRABROWSERIMPL_HEADER_

