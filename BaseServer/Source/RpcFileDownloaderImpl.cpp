#include "RpcFileDownloaderImpl.h"
#include "SQLiteUtil.h"
#include "Datetime.h"
#include "PathUtils.h"

#include <Ice/Ice.h>

#include <assert.h>

RpcFileDownloaderImpl::RpcFileDownloaderImpl() : destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcFileDownloaderImpl::~RpcFileDownloaderImpl()
{
}

Rpc::ErrorCode RpcFileDownloaderImpl::init(const std::string& filename)
{
	std::string fn = normalizePath(filename);

	stream_.reset(new std::fstream(fn.c_str(), std::ios::in|std::ios::binary));
	if (!stream_->is_open()) {
		return Rpc::ec_file_io_error;
	}

	stream_->seekg(0, std::ios::end);
	size_ = stream_->tellg();
	stream_->seekg(0, std::ios::beg);

	filename_ = filename;

	return Rpc::ec_success;
}

const std::string& RpcFileDownloaderImpl::filename()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return filename_;
}

bool RpcFileDownloaderImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcFileDownloaderImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcFileDownloaderImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcFileDownloaderImpl::getSize(Ice::Long& size, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	size = size_;
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcFileDownloaderImpl::read(Ice::Long offset, Ice::Int size, Rpc::ByteSeq& bytes, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (offset + size > size_) {
		return Rpc::ec_out_of_range;
	}

	if (!stream_->seekg(offset)) {
		return Rpc::ec_file_io_error;
	}

	bytes.resize(size);

	if (!stream_->read((char*)&bytes[0], size)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
}

void RpcFileDownloaderImpl::finish(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	stream_.reset();

	finished_ = true;
}

void RpcFileDownloaderImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	stream_.reset();

	cancelled_ = true;
}

void RpcFileDownloaderImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

