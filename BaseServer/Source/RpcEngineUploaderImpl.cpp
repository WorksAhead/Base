#include "RpcEngineUploaderImpl.h"
#include "SQLiteUtil.h"
#include "Datetime.h"

#include <Crc.h>

#include <Ice/Ice.h>

RpcEngineUploaderImpl::RpcEngineUploaderImpl(CenterPtr center) : center_(center), EngineVerLocked_(false)
{
}

RpcEngineUploaderImpl::~RpcEngineUploaderImpl()
{
	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_write);
		EngineVerLocked_ = false;
	}
}

Rpc::ErrorCode RpcEngineUploaderImpl::init(const std::string& name, const std::string& version, const std::string& info)
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

	const std::string& filename = center_->engineFileName(name, version);
	stream_.reset(new std::fstream(filename.c_str(), std::ios::out|std::ios::binary));

	if (!stream_->is_open()) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineUploaderImpl::write(Ice::Long offset, const std::pair<const Ice::Byte*, const Ice::Byte*>& bytes, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	if (!stream_->seekp(offset)) {
		return Rpc::ec_file_io_error;
	}

	if (!stream_->write((const char*)bytes.first, bytes.second - bytes.first)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineUploaderImpl::finish(Ice::Int crc32, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);

	if (!stream_->flush()) {
		return Rpc::ec_file_io_error;
	}

	const std::string& filename = center_->engineFileName(name_, version_);
	stream_.reset(new std::fstream(filename.c_str(), std::ios::in|std::ios::binary));

	if (!stream_->is_open()) {
		return Rpc::ec_file_io_error;
	}

	if (!stream_->seekg(0, std::ios::end)) {
		return Rpc::ec_file_io_error;
	}

	const auto len = stream_->tellp();

	if (!stream_->seekg(0, std::ios::beg)) {
		return Rpc::ec_file_io_error;
	}

	if (static_cast<uint32_t>(crc32) != calculateCRC(*stream_, len)) {
		return Rpc::ec_file_data_error;
	}

	stream_.reset();

	center_->addEngineVersion(name_, version_, info_);

	if (EngineVerLocked_) {
		center_->unlockEngineVersion(name_, version_, Center::lock_write);
		EngineVerLocked_ = false;
	}

	c.adapter->remove(c.id);

	return Rpc::ec_success;
}

