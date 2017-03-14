#include "RpcExtraSubmitterImpl.h"
#include "PathUtils.h"

#include <Ice/Ice.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <list>
#include <set>
#include <assert.h>

namespace fs = boost::filesystem;

RpcExtraSubmitterImpl::RpcExtraSubmitterImpl(ContextPtr context)
	: context_(context), destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcExtraSubmitterImpl::~RpcExtraSubmitterImpl()
{
}

Rpc::ErrorCode RpcExtraSubmitterImpl::init(int mode, const std::string& id)
{
	if (mode == submit_mode)
	{
		id_ = id.empty() ? context_->center()->generateUuid() : id;
		base_ = context_->center()->getExtraPath(id_);

		boost::system::error_code ec;
		if (!fs::create_directories(makeSafePath(base_), ec)) {
			return Rpc::ec_file_io_error;
		}

		mode_ = mode;

		return Rpc::ec_success;
	}
	else
	{
		id_ = id;
		mode_ = mode;

		return Rpc::ec_success;
	}
}

bool RpcExtraSubmitterImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcExtraSubmitterImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcExtraSubmitterImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcExtraSubmitterImpl::setTitle(const std::string& title, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Title"] = title;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::setCategory(const std::string& category, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	std::vector<std::string> list;

	boost::split(list, category, boost::is_any_of(","));

	std::string formattedCategory;

	for (std::string& s : list) {
		boost::trim(s);
		if (!formattedCategory.empty()) {
			formattedCategory += ",";
		}
		formattedCategory += "(" + s + ")";
	}

	form_["Category"] = formattedCategory;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::setSetup(const std::string& setup, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Setup"] = setup;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::setInfo(const std::string& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Info"] = info;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::uploadImage(Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (imageUploader_.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr ptr = new RpcFileUploaderImpl;

	fs::path filename = base_;
	filename /= "image_0";

	Rpc::ErrorCode ec = ptr->init(filename.string());
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(ptr));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	imageUploader_.first = uploaderPrx;
	imageUploader_.second = ptr;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::uploadExtra(Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (extraUploader_.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr ptr = new RpcFileUploaderImpl;

	fs::path filename = base_;
	filename /= "extra";

	Rpc::ErrorCode ec = ptr->init(filename.string());
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(ptr));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	extraUploader_.first = uploaderPrx;
	extraUploader_.second = ptr;

	return Rpc::ec_success;
}


void RpcExtraSubmitterImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	if (extraUploader_.first) {
		extraUploader_.first->cancel();
	}

	if (imageUploader_.first) {
		imageUploader_.first->cancel();
	}

	cancelled_ = true;
}

Rpc::ErrorCode RpcExtraSubmitterImpl::finish(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	setEmptyIfNotExist(form_, "Info");

	if (!form_.count("Title") || !form_.count("Setup")) {
		return Rpc::ec_incomplete_form;
	}

	if (mode_ == submit_mode)
	{
		if (!extraUploader_.second || !extraUploader_.second->isFinished()) {
			return Rpc::ec_incomplete_extra;
		}

		if (!imageUploader_.second || !imageUploader_.second->isFinished()) {
			return Rpc::ec_incomplete_extra;
		}

		form_["User"] = context_->user();

		context_->center()->addExtra(form_, id_);
	}
	else
	{
		context_->center()->updateExtra(form_, id_);
	}

	return Rpc::ec_success;
}

void RpcExtraSubmitterImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

