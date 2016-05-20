#include "RpcSessionImpl.h"
#include "RpcEngineBrowserImpl.h"
#include "RpcEngineUploaderImpl.h"
#include "RpcEngineDownloaderImpl.h"

#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

RpcSessionImpl::RpcSessionImpl(CenterPtr center)
	: destroy_(false), timestamp_(IceUtil::Time::now(IceUtil::Time::Monotonic)), center_(center)
{
}

RpcSessionImpl::~RpcSessionImpl()
{
}

void RpcSessionImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	destroy_ = true;

	try {
		c.adapter->remove(c.id);
		for (const Ice::Identity& id : ids_) {
			if (c.adapter->find(id)) {
				c.adapter->remove(id);
			}
		}
	}
	catch(const Ice::ObjectAdapterDeactivatedException&) {
	}

	ids_.clear();
}

void RpcSessionImpl::refresh(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
}

Rpc::ErrorCode RpcSessionImpl::browseEngines(Rpc::EngineBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineBrowserImplPtr browser = new RpcEngineBrowserImpl(center_);

	Rpc::ErrorCode ec = browser->init();
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::EngineBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	ids_.insert(browserPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::uploadEngine(const std::string& name, const std::string& version, const std::string& info, Rpc::EngineUploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineUploaderImplPtr uploader = new RpcEngineUploaderImpl(center_);

	Rpc::ErrorCode ec = uploader->init(name, version, info);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::EngineUploaderPrx::uncheckedCast(c.adapter->addWithUUID(uploader));

	ids_.insert(uploaderPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::downloadEngine(const std::string& name, const std::string& version, Rpc::EngineDownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineDownloaderImplPtr downloader = new RpcEngineDownloaderImpl(center_);

	Rpc::ErrorCode ec = downloader->init(name, version);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	downloaderPrx = Rpc::EngineDownloaderPrx::uncheckedCast(c.adapter->addWithUUID(downloader));

	ids_.insert(downloaderPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeEngine(const std::string& name, const std::string& version, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	EngineVersionLockGuard engineLock(center_.get(), name, version, Center::lock_write);

	if (!engineLock.isLocked()) {
		return Rpc::ec_engine_version_is_locked;
	}

	std::string state;
	if (!center_->getEngineVersionState(name, version, state)) {
		return Rpc::ec_engine_version_does_not_exist;
	}

	if (state != "Normal") {
		if (state == "Removed") {
			return Rpc::ec_engine_version_is_removed;
		}
		assert(false);
	}

	center_->changeEngineVersionState(name, version, "Removed");

	const std::string& filename = center_->engineFileName(name, version);

	if (!fs::remove(filename)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

IceUtil::Time RpcSessionImpl::timestamp()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	return timestamp_;
}

void RpcSessionImpl::checkIsDestroyed()
{
	if (destroy_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

