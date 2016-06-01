#include "RpcEngineVersionBrowserImpl.h"

#include <Ice/Ice.h>

RpcEngineVersionBrowserImpl::RpcEngineVersionBrowserImpl(CenterPtr center) : center_(center)
{
}

RpcEngineVersionBrowserImpl::~RpcEngineVersionBrowserImpl()
{
}

Rpc::ErrorCode RpcEngineVersionBrowserImpl::init()
{
	s_.reset(new SQLite::Statement(*center_->db(), "SELECT * FROM EngineVersions ORDER BY UpTime DESC"));

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineVersionBrowserImpl::next(Ice::Int n, Rpc::EngineVersionItemSeq& items, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	items.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::EngineVersionItem item;
		item.name = s_->getColumn("Name").getText();
		item.version = s_->getColumn("Version").getText();
		item.uptime = s_->getColumn("UpTime").getText();
		item.info = s_->getColumn("Info").getText();
		item.state = s_->getColumn("State").getText();

		items.push_back(item);
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineVersionBrowserImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	c.adapter->remove(c.id);
	return Rpc::ec_success;
}

