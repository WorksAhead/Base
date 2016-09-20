#include "RpcEngineVersionDownloaderImpl.h"

#include <assert.h>

RpcEngineVersionDownloaderImpl::RpcEngineVersionDownloaderImpl(CenterPtr center) : center_(center)
{
}

RpcEngineVersionDownloaderImpl::~RpcEngineVersionDownloaderImpl()
{
	unlockEngineVersionIfLocked();
}

Rpc::ErrorCode RpcEngineVersionDownloaderImpl::init(const std::string& name, const std::string& version)
{
	name_ = name;
	version_ = version;

	if (!center_->lockEngineVersion(name_, version_, Center::lock_read)) {
		return Rpc::ec_engine_version_is_locked;
	}

	EngineVerLocked_ = true;

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

	return RpcFileDownloaderImpl::init(center_->getEnginePath(name, version));
}

void RpcEngineVersionDownloaderImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	RpcFileDownloaderImpl::finish(c);
	unlockEngineVersionIfLocked();
}

void RpcEngineVersionDownloaderImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	RpcFileDownloaderImpl::cancel(c);
	unlockEngineVersionIfLocked();
}

void RpcEngineVersionDownloaderImpl::unlockEngineVersionIfLocked()
{
	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_read);
		EngineVerLocked_ = false;
	}
}

