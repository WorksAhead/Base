#include "RpcExtraBrowserImpl.h"

#include <Ice/Ice.h>

RpcExtraBrowserImpl::RpcExtraBrowserImpl(CenterPtr center) : center_(center), destroyed_(false)
{

}

RpcExtraBrowserImpl::~RpcExtraBrowserImpl()
{

}

Rpc::ErrorCode RpcExtraBrowserImpl::init()
{
	std::ostringstream oss;
	oss << "SELECT * FROM Extras";
	oss << " WHERE State=" << sqlText("Normal");
	oss << " ORDER BY UpTime DESC";

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
		info.title = s_->getColumn("Title").getText();
		info.setup = s_->getColumn("Setup").getText();
		info.user = s_->getColumn("User").getText();
		info.uptime = s_->getColumn("UpTime").getText();
		info.info = s_->getColumn("Info").getText();
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

