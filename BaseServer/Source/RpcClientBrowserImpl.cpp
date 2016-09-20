#include "RpcClientBrowserImpl.h"

#include <Ice/Ice.h>

RpcClientBrowserImpl::RpcClientBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{
}

RpcClientBrowserImpl::~RpcClientBrowserImpl()
{
}

Rpc::ErrorCode RpcClientBrowserImpl::init()
{
	std::ostringstream oss;
	oss << "SELECT * FROM Clients";
	oss << " ORDER BY UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcClientBrowserImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	destroyed_ = true;

	try {
		c.adapter->remove(c.id);
	}
	catch (const Ice::ObjectAdapterDeactivatedException&) {
	}
}

Rpc::ErrorCode RpcClientBrowserImpl::next(Ice::Int n, Rpc::ClientInfoSeq& items, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	items.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::ClientInfo item;
		item.version = s_->getColumn("Version").getText();
		item.uptime = s_->getColumn("UpTime").getText();
		item.info = s_->getColumn("Info").getText();
		item.state = s_->getColumn("State").getText();

		items.push_back(item);
	}

	return Rpc::ec_success;
}

void RpcClientBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

