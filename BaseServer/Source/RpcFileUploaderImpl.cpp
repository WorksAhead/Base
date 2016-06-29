#include "RpcFileUploaderImpl.h"

#include <Crc.h>

#include <Ice/Ice.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

RpcFileUploaderImpl::RpcFileUploaderImpl() : destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcFileUploaderImpl::~RpcFileUploaderImpl()
{
	if (stream_) {
		stream_.reset();
	}
	if (!finished_ && !filename_.empty()) {
		boost::system::error_code ec;
		fs::remove(filename_, ec);
	}
}

Rpc::ErrorCode RpcFileUploaderImpl::init(const std::string& filename)
{
	stream_.reset(new std::fstream(filename.c_str(), std::ios::out|std::ios::binary));
	if (!stream_->is_open()) {
		return Rpc::ec_file_io_error;
	}

	filename_ = filename;

	return Rpc::ec_success;
}

const std::string& RpcFileUploaderImpl::filename()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return filename_;
}

bool RpcFileUploaderImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcFileUploaderImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcFileUploaderImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcFileUploaderImpl::write(Ice::Long offset, const std::pair<const Ice::Byte*, const Ice::Byte*>& bytes, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (!stream_->seekp(offset)) {
		return Rpc::ec_file_io_error;
	}

	if (!stream_->write((const char*)bytes.first, bytes.second - bytes.first)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcFileUploaderImpl::finish(Ice::Int crc32, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (!stream_->flush()) {
		return Rpc::ec_file_io_error;
	}

	stream_.reset(new std::fstream(filename_.c_str(), std::ios::in|std::ios::binary));

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

	finished_ = true;

	return Rpc::ec_success;
}

void RpcFileUploaderImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	stream_.reset();

	cancelled_ = true;
}

void RpcFileUploaderImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

