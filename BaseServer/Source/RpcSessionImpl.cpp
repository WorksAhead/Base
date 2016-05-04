#include "RpcSessionImpl.h"

#include <Ice/Ice.h>

RpcSessionImpl::RpcSessionImpl(DatabasePtr db)
	: destroy_(false), timestamp_(IceUtil::Time::now(IceUtil::Time::Monotonic)), db_(db)
{
}

RpcSessionImpl::~RpcSessionImpl()
{
}

void RpcSessionImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	c.adapter->remove(c.id);
	destroy_ = true;
}

void RpcSessionImpl::refresh(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
}

IceUtil::Time RpcSessionImpl::timestamp()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	return timestamp_;
}

void RpcSessionImpl::checkIsDestroyed()
{
	if (destroy_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

