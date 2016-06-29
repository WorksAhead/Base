#ifndef RPCOBJECTMANAGER_HEADER_
#define RPCOBJECTMANAGER_HEADER_

#include <RpcManagedObject.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <map>

class RpcObjectManager {
public:
	RpcObjectManager();
	~RpcObjectManager();

	bool addObject(Rpc::ManagedObjectPrx);

	void destroyAllObjects(const Ice::Current&);
	void refresh(const Ice::Current&);

private:
	std::map<Ice::Identity, Rpc::ManagedObjectPrx> objs_;
	boost::recursive_mutex sync_;
};

typedef boost::shared_ptr<RpcObjectManager> RpcObjectManagerPtr;

#endif // RPCOBJECTMANAGER_HEADER_

