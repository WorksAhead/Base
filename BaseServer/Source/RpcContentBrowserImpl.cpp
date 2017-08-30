#include "RpcContentBrowserImpl.h"

#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>

RpcContentBrowserImpl::RpcContentBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{
}

RpcContentBrowserImpl::~RpcContentBrowserImpl()
{
}

Rpc::ErrorCode RpcContentBrowserImpl::init(const std::string& page, const std::string& category, const std::string& search)
{
	std::ostringstream oss;
	oss << "SELECT Id, Title, State FROM Contents";

	bool firstCondition = true;

	if (!page.empty())
	{
		if (firstCondition) {
			oss << " WHERE ";
			firstCondition = false;
		}
		else {
			oss << " AND ";
		}

		oss << "Page LIKE " << sqlText("%(" + page + ")%");
	}

	if (!category.empty())
	{
		std::map<std::string, std::string> categories;
		center_->getGroupedContentCategories(categories);

		std::vector<std::string> list;
		boost::split(list, category, boost::is_any_of(","));

		std::map<std::string, std::string> groupedExp;

		for (const std::string& s : list)
		{
			auto it = categories.find(s);

			if (it != categories.end())
			{
				std::string& exp = groupedExp[it->second];

				if (!exp.empty()) {
					exp += " OR ";
				}

				exp += "Category LIKE " + sqlText("%(" + s + ")%");
			}
		}

		if (firstCondition) {
			oss << " WHERE ";
			firstCondition = false;
		}
		else {
			oss << " AND ";
		}

		for (auto& p : groupedExp)
		{
			oss << "(" + p.second + ")";
		}
	}

	if (!search.empty())
	{
		if (firstCondition) {
			oss << " WHERE ";
			firstCondition = false;
		}
		else {
			oss << " AND ";
		}

		oss << "(";
		oss << "Title LIKE " << sqlText("%" + search + "%");
		oss << " OR Desc LIKE " << sqlText("%" + search + "%");
		oss << ")";
	}

	oss << " ORDER BY UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentBrowserImpl::init(const std::string& parentId)
{
	std::ostringstream oss;
	oss << "SELECT Id, Title, State FROM Contents";
	oss << " WHERE ParentId=" << sqlText(parentId);
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
		item.state = s_->getColumn("State").getText();

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

