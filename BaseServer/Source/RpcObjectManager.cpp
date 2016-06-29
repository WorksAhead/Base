#include "RpcObjectManager.h"

#include <Ice/Ice.h>

RpcObjectManager::RpcObjectManager()
{
}

RpcObjectManager::~RpcObjectManager()
{
}

bool RpcObjectManager::addObject(Rpc::ManagedObjectPrx obj)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	objs_.insert(std::make_pair(obj->ice_getIdentity(), obj));
	return true;
}

void RpcObjectManager::destroyAllObjects(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	auto it = objs_.begin();

	while (it != objs_.end()) {
		if (c.adapter->find(it->first)) {
			it->second->destroy();
		}
		it = objs_.erase(it);
	}
}

void RpcObjectManager::refresh(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	auto it = objs_.begin();

	while (it != objs_.end()) {
		if (c.adapter->find(it->first)) {
			++it;
		}
		else {
			it = objs_.erase(it);
		}
	}
}

