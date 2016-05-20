#include "RpcEngineDownloaderImpl.h"
#include "SQLiteUtil.h"
#include "Datetime.h"

#include <Ice/Ice.h>

#include <assert.h>

RpcEngineDownloaderImpl::RpcEngineDownloaderImpl(CenterPtr center) : center_(center)
{
}

RpcEngineDownloaderImpl::~RpcEngineDownloaderImpl()
{
	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_read);
		EngineVerLocked_ = false;
	}
}

Rpc::ErrorCode RpcEngineDownloaderImpl::init(const std::string& name, const std::string& version)
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

	const std::string& filename = center_->engineFileName(name, version);
	stream_.reset(new std::fstream(filename.c_str(), std::ios::in|std::ios::binary));

	if (!stream_->is_open()) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineDownloaderImpl::read(Ice::Long offset, Ice::Int num, Rpc::ByteSeq& bytes, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	if (!stream_->seekg(offset)) {
		return Rpc::ec_file_io_error;
	}

	bytes.resize(num);

	if (!stream_->read((char*)&bytes[0], num)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineDownloaderImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	stream_.reset();

	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_read);
		EngineVerLocked_ = false;
	}

	c.adapter->remove(c.id);

	return Rpc::ec_success;
}

