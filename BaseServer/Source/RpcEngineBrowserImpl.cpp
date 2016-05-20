#include "RpcEngineBrowserImpl.h"

#include <Ice/Ice.h>

RpcEngineBrowserImpl::RpcEngineBrowserImpl(CenterPtr center) : center_(center)
{
}

RpcEngineBrowserImpl::~RpcEngineBrowserImpl()
{
}

Rpc::ErrorCode RpcEngineBrowserImpl::init()
{
	s_.reset(new SQLite::Statement(*center_->db(), "SELECT * FROM Engines ORDER BY UpTime DESC"));

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineBrowserImpl::next(Ice::Int n, Rpc::EngineItemSeq& items, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	items.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::EngineItem item;
		item.name = s_->getColumn("Name").getText();
		item.version = s_->getColumn("Version").getText();
		item.uptime = s_->getColumn("UpTime").getText();
		item.info = s_->getColumn("Info").getText();
		item.state = s_->getColumn("State").getText();

		items.push_back(item);
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineBrowserImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	c.adapter->remove(c.id);
	return Rpc::ec_success;
}

