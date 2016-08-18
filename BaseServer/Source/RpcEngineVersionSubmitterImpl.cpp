#include "RpcEngineVersionSubmitterImpl.h"

#include <Ice/Ice.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <list>
#include <set>
#include <assert.h>

namespace fs = boost::filesystem;

RpcEngineVersionSubmitterImpl::RpcEngineVersionSubmitterImpl(ContextPtr context)
	: context_(context), EngineVerLocked_(false), destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcEngineVersionSubmitterImpl::~RpcEngineVersionSubmitterImpl()
{
	unlockEngineVersionIfLocked();
}

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::init(const std::string& name, const std::string& version, int mode)
{
	name_ = name;
	version_ = version;
	mode_ = mode;

	if (!context_->center()->lockEngineVersion(name_, version_, Center::lock_write)) {
		return Rpc::ec_engine_version_is_locked;
	}

	EngineVerLocked_ = true;

	if (mode_ == submit_mode) {
		std::string state;
		if (context_->center()->getEngineVersionState(name, version, state)) {
			return Rpc::ec_engine_version_already_exists;
		}
	}
	else {
		std::string state;
		if (!context_->center()->getEngineVersionState(name, version, state)) {
			return Rpc::ec_engine_version_does_not_exist;
		}
		if (state != "Normal") {
			if (state == "Removed") {
				return Rpc::ec_engine_version_is_removed;
			}
			assert(false);
		}
	}

	return Rpc::ec_success;
}

bool RpcEngineVersionSubmitterImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcEngineVersionSubmitterImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcEngineVersionSubmitterImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::setSetup(const std::string& setup, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Setup"] = setup;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::setUnSetup(const std::string& unsetup, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["UnSetup"] = unsetup;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::setInfo(const std::string& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Info"] = info;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::uploadEngine(Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (engineUploader_.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr uploader = new RpcFileUploaderImpl;

	Rpc::ErrorCode ec = uploader->init(context_->center()->getEnginePath(name_, version_));
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(uploader));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	engineUploader_.first = uploaderPrx;
	engineUploader_.second = uploader;

	return Rpc::ec_success;
}

void RpcEngineVersionSubmitterImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	cancelled_ = true;

	if (engineUploader_.second) {
		engineUploader_.second->cancel(c);
	}

	unlockEngineVersionIfLocked();
}

Rpc::ErrorCode RpcEngineVersionSubmitterImpl::finish(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	setEmptyIfNotExist(form_, "Setup");
	setEmptyIfNotExist(form_, "UnSetup");
	setEmptyIfNotExist(form_, "Info");

	if (mode_ == submit_mode)
	{
		if (!engineUploader_.second || !engineUploader_.second->isFinished()) {
			return Rpc::ec_incomplete_engine_version;
		}

		if (!context_->center()->addEngineVersion(name_, version_, form_)) {
			return Rpc::ec_operation_failed;
		}
	}
	else
	{
		if (!context_->center()->updateEngineVersion(name_, version_, form_)) {
			return Rpc::ec_operation_failed;
		}
	}

	unlockEngineVersionIfLocked();

	return Rpc::ec_success;
}

void RpcEngineVersionSubmitterImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

void RpcEngineVersionSubmitterImpl::unlockEngineVersionIfLocked()
{
	if (EngineVerLocked_) {
		context_->center()->unlockEngineVersion(name_, version_, Center::lock_write);
		EngineVerLocked_ = false;
	}
}

