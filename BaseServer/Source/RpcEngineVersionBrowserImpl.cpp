#include "RpcEngineVersionBrowserImpl.h"

#include <Ice/Ice.h>

RpcEngineVersionBrowserImpl::RpcEngineVersionBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{
}

RpcEngineVersionBrowserImpl::~RpcEngineVersionBrowserImpl()
{
}

Rpc::ErrorCode RpcEngineVersionBrowserImpl::init(bool all)
{
	std::ostringstream oss;
	oss << "SELECT * FROM EngineVersions";

	if (!all) {
		oss << " WHERE State=" << sqlText("Normal");
	}

	oss << " ORDER BY DisplayPriority DESC, UpTime DESC";

	s_.reset(new SQLite::Statement(*center_->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcEngineVersionBrowserImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcEngineVersionBrowserImpl::next(Ice::Int n, Rpc::EngineVersionSeq& seq, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	seq.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::EngineVersionInfo info;
		info.name = s_->getColumn("Name").getText();
		info.version = s_->getColumn("Version").getText();
		info.setup = s_->getColumn("Setup").getText();
		info.unsetup = s_->getColumn("UnSetup").getText();
		info.uptime = s_->getColumn("UpTime").getText();
		info.info = s_->getColumn("Info").getText();
		info.displayPriority = s_->getColumn("DisplayPriority").getInt();
		info.state = s_->getColumn("State").getText();

		seq.push_back(info);
	}

	return Rpc::ec_success;
}

void RpcEngineVersionBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

