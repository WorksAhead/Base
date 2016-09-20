#include "RpcClientSubmitterImpl.h"

#include <Ice/Ice.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <list>
#include <set>
#include <assert.h>

namespace fs = boost::filesystem;

RpcClientSubmitterImpl::RpcClientSubmitterImpl(ContextPtr context)
	: context_(context), clientVerLocked_(false), destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcClientSubmitterImpl::~RpcClientSubmitterImpl()
{
	unlockClientVersionIfLocked();
}

Rpc::ErrorCode RpcClientSubmitterImpl::init(const std::string& version, int mode)
{
	version_ = version;
	mode_ = mode;

	if (!context_->center()->lockClientVersion(version, Center::lock_write)) {
		return Rpc::ec_client_version_is_locked;
	}

	clientVerLocked_ = true;

	if (mode_ == submit_mode) {
		std::string state;
		if (context_->center()->getClientVersionState(version, state)) {
			return Rpc::ec_client_version_already_exists;
		}
	}
	else {
		std::string state;
		if (!context_->center()->getClientVersionState(version, state)) {
			return Rpc::ec_client_version_does_not_exist;
		}
	}

	return Rpc::ec_success;
}

bool RpcClientSubmitterImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcClientSubmitterImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcClientSubmitterImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcClientSubmitterImpl::setInfo(const std::string& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Info"] = info;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcClientSubmitterImpl::uploadClient(Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (clientUploader_.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr uploader = new RpcFileUploaderImpl;

	Rpc::ErrorCode ec = uploader->init(context_->center()->getClientPath(version_));
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(uploader));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	clientUploader_.first = uploaderPrx;
	clientUploader_.second = uploader;

	return Rpc::ec_success;
}

void RpcClientSubmitterImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	cancelled_ = true;

	if (clientUploader_.second) {
		clientUploader_.second->cancel(c);
	}

	unlockClientVersionIfLocked();
}

Rpc::ErrorCode RpcClientSubmitterImpl::finish(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	setEmptyIfNotExist(form_, "Info");

	if (mode_ == submit_mode)
	{
		if (!clientUploader_.second || !clientUploader_.second->isFinished()) {
			return Rpc::ec_incomplete_client_version;
		}

		if (!context_->center()->addClientVersion(version_, form_)) {
			return Rpc::ec_operation_failed;
		}
	}
	else
	{
		if (!context_->center()->updateClientVersion(version_, form_)) {
			return Rpc::ec_operation_failed;
		}
	}

	unlockClientVersionIfLocked();

	return Rpc::ec_success;
}

void RpcClientSubmitterImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

void RpcClientSubmitterImpl::unlockClientVersionIfLocked()
{
	if (clientVerLocked_) {
		context_->center()->unlockClientVersion(version_, Center::lock_write);
		clientVerLocked_ = false;
	}
}

