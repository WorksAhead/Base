#include "RpcExtraBrowserImpl.h"

#include <Ice/Ice.h>

RpcExtraBrowserImpl::RpcExtraBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{

}

RpcExtraBrowserImpl::~RpcExtraBrowserImpl()
{

}

Rpc::ErrorCode RpcExtraBrowserImpl::init(const std::string& category, const std::string& search)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Extras";

	if (!category.empty())
	{
		std::map<std::string, std::string> categories;
		center_->getGroupedExtraCategories(categories);

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
		oss << " OR Info LIKE " << sqlText("%" + search + "%");
		oss << ")";
	}

	oss << " ORDER BY DisplayPriority DESC, UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraBrowserImpl::init(const std::string& parentId)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Extras";
	oss << " WHERE ParentId=" << sqlText(parentId);
	oss << " ORDER BY DisplayPriority DESC, UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcExtraBrowserImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcExtraBrowserImpl::next(Ice::Int n, Rpc::ExtraInfoSeq& seq, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	seq.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::ExtraInfo info;
		info.id = s_->getColumn("Id").getText();
		info.parentId = s_->getColumn("ParentId").getText();
		info.title = s_->getColumn("Title").getText();

		std::string category = s_->getColumn("Category").getText();
		category.erase(std::remove_if(category.begin(), category.end(), boost::is_any_of("()")), category.end());
		info.category = category;

		info.setup = s_->getColumn("Setup").getText();
		info.user = s_->getColumn("User").getText();
		info.uptime = s_->getColumn("UpTime").getText();
		info.info = s_->getColumn("Info").getText();
		info.displayPriority = s_->getColumn("DisplayPriority").getInt();
		info.state = s_->getColumn("State").getText();

		seq.push_back(info);
	}

	return Rpc::ec_success;
}

void RpcExtraBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

