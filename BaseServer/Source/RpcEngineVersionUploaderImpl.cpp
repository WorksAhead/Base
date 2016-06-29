#include "RpcEngineVersionUploaderImpl.h"

RpcEngineVersionUploaderImpl::RpcEngineVersionUploaderImpl(CenterPtr center) : center_(center), EngineVerLocked_(false)
{
}

RpcEngineVersionUploaderImpl::~RpcEngineVersionUploaderImpl()
{
	unlockEngineVersionIfLocked();
}

Rpc::ErrorCode RpcEngineVersionUploaderImpl::init(const std::string& name, const std::string& version, const std::string& info)
{
	name_ = name;
	version_ = version;
	info_ = info;

	if (!center_->lockEngineVersion(name_, version_, Center::lock_write)) {
		return Rpc::ec_engine_version_is_locked;
	}

	EngineVerLocked_ = true;

	std::string state;
	if (center_->getEngineVersionState(name, version, state)) {
		return Rpc::ec_engine_version_already_exists;
	}

	return RpcFileUploaderImpl::init(center_->getEnginePath(name, version));
}

Rpc::ErrorCode RpcEngineVersionUploaderImpl::finish(Ice::Int crc32, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	Rpc::ErrorCode ec = RpcFileUploaderImpl::finish(crc32, c);

	if (ec != Rpc::ec_success) {
		return ec;
	}

	center_->addEngineVersion(name_, version_, info_);
	unlockEngineVersionIfLocked();

	return Rpc::ec_success;
}

void RpcEngineVersionUploaderImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcFileUploaderImpl::cancel(c);
	unlockEngineVersionIfLocked();
}

void RpcEngineVersionUploaderImpl::unlockEngineVersionIfLocked()
{
	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_write);
		EngineVerLocked_ = false;
	}
}

