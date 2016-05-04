#ifndef BASESERVER_RPCSESSIONIMPL_HEADER_
#define BASESERVER_RPCSESSIONIMPL_HEADER_

#include "SQLiteUtil.h"

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>

#include <boost/thread/recursive_mutex.hpp>

class RpcSessionImpl : public Rpc::Session {
public:
	explicit RpcSessionImpl(DatabasePtr);
	~RpcSessionImpl();

	virtual void destroy(const Ice::Current&);
	virtual void refresh(const Ice::Current&);

	IceUtil::Time timestamp();

private:
	void checkIsDestroyed();

private:
	bool destroy_;
	IceUtil::Time timestamp_;
	DatabasePtr db_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcSessionImpl> RpcSessionImplPtr;

#endif // BASESERVER_RPCSESSIONIMPL_HEADER_

