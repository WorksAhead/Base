#include "RpcUserBrowserImpl.h"

#include <Ice/Ice.h>

RpcUserBrowserImpl::RpcUserBrowserImpl(ContextPtr context) : context_(context), destroyed_(false)
{
}

RpcUserBrowserImpl::~RpcUserBrowserImpl()
{
}

Rpc::ErrorCode RpcUserBrowserImpl::init()
{
	std::ostringstream oss;
	oss << "SELECT * FROM Users";
	oss << " ORDER BY Username";

	s_.reset(new SQLite::Statement(*context_->center()->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcUserBrowserImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcUserBrowserImpl::next(Ice::Int n, Rpc::UserSeq& users, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	users.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::User u;
		u.username = s_->getColumn("Username").getText();
		u.group = s_->getColumn("Group").getText();
		u.regTime = s_->getColumn("RegTime").getText();
		u.info = s_->getColumn("Info").getText();

		users.push_back(u);
	}

	return Rpc::ec_success;
}

void RpcUserBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

