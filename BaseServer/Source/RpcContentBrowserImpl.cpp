#include "RpcContentBrowserImpl.h"

#include <Ice/Ice.h>

RpcContentBrowserImpl::RpcContentBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{
}

RpcContentBrowserImpl::~RpcContentBrowserImpl()
{
}

Rpc::ErrorCode RpcContentBrowserImpl::init(const std::string& page, const std::string& category)
{
	std::ostringstream oss;
	oss << "SELECT Id, Title FROM Contents";

	if (!page.empty() || !category.empty()) {
		oss << " WHERE";
		if (!page.empty() && !category.empty()) {
			oss << " Page=" << sqlText(page);
			oss << " AND Category=" << sqlText(category);
		}
		else if (!page.empty()) {
			oss << " Page=" << sqlText(page);
		}
		else {
			oss << " Category=" << sqlText(category);
		}
	}

	oss << " ORDER BY UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcContentBrowserImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcContentBrowserImpl::next(Ice::Int n, Rpc::ContentItemSeq& items, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	items.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::ContentItem item;
		item.id = s_->getColumn("Id").getText();
		item.title = s_->getColumn("Title").getText();

		items.push_back(item);
	}

	return Rpc::ec_success;
}

void RpcContentBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

