#include "RpcClientDownloaderImpl.h"

RpcClientDownloaderImpl::RpcClientDownloaderImpl(CenterPtr center) : center_(center)
{
	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
}

RpcClientDownloaderImpl::~RpcClientDownloaderImpl()
{
	unlockClientVersionIfLocked();
}

Rpc::ErrorCode RpcClientDownloaderImpl::init(const std::string& version)
{
	version_ = version;

	if (!center_->lockClientVersion(version_, Center::lock_read)) {
		return Rpc::ec_client_version_is_locked;
	}

	clientVerLocked_ = true;

	return RpcFileDownloaderImpl::init(center_->getClientPath(version_));
}

Rpc::ErrorCode RpcClientDownloaderImpl::read(Ice::Long offset, Ice::Int size, Rpc::ByteSeq& bytes, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	Rpc::ErrorCode ec = RpcFileDownloaderImpl::read(offset, size, bytes, c);
	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
	return ec;
}

void RpcClientDownloaderImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	RpcFileDownloaderImpl::finish(c);
	unlockClientVersionIfLocked();
}

void RpcClientDownloaderImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	RpcFileDownloaderImpl::cancel(c);
	unlockClientVersionIfLocked();
}

IceUtil::Time RpcClientDownloaderImpl::timestamp()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return timestamp_;
}

void RpcClientDownloaderImpl::unlockClientVersionIfLocked()
{
	if (clientVerLocked_) {
		center_->unlockClientVersion(version_, Center::lock_read);
		clientVerLocked_ = false;
	}
}

