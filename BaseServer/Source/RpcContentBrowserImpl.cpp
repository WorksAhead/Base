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
	oss << "SELECT Id, Title FROM Contents";
	oss << " WHERE State=" << sqlText("Normal");

	if (!page.empty()) {
		oss << " AND Page LIKE " << sqlText("%(" + page + ")%");
	}

	if (!category.empty())
	{
		std::map<std::string, std::string> categories;
		center_->getGroupedContentCategories(categories);

		std::vector<std::string> list;
		boost::split(list, category, boost::is_any_of(","));

		std::map<std::string, std::string> groupedExp;

		for (const std::string& s : list) {
			auto it = categories.find(s);
			if (it != categories.end()) {
				std::string& exp = groupedExp[it->second];
				if (!exp.empty()) {
					exp += " OR ";
				}
				exp += "Category LIKE " + sqlText("%(" + s + ")%");
			}
		}

		for (auto& p : groupedExp) {
			oss << " AND (" + p.second + ")";
		}
	}

	if (!search.empty())
	{
		oss << " AND (";
		oss << "Title LIKE " << sqlText("%" + search + "%");
		oss << " OR Desc LIKE " << sqlText("%" + search + "%");
		oss << ")";
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

