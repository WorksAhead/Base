#include "RpcContentSubmitterImpl.h"
#include "PathUtils.h"

#include <Ice/Ice.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <list>
#include <set>
#include <limits>
#include <assert.h>

namespace fs = boost::filesystem;

RpcContentSubmitterImpl::RpcContentSubmitterImpl(ContextPtr context)
	: context_(context), destroyed_(false), finished_(false), cancelled_(false)
{
}

RpcContentSubmitterImpl::~RpcContentSubmitterImpl()
{
}

Rpc::ErrorCode RpcContentSubmitterImpl::init(int mode, const std::string& id)
{
	if (mode == submit_mode)
	{
		id_ = id.empty() ? context_->center()->generateUuid() : id;
		base_ = context_->center()->getContentPath(id_);

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

bool RpcContentSubmitterImpl::isFinished()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return finished_;
}

bool RpcContentSubmitterImpl::isCancelled()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	return cancelled_;
}

void RpcContentSubmitterImpl::destroy(const Ice::Current& c)
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

Rpc::ErrorCode RpcContentSubmitterImpl::setTitle(const std::string& title, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Title"] = title;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setPage(const std::string& page, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	std::set<std::string> pageSet;
	{
		std::vector<std::string> v;
		context_->center()->getPages(v);
		for (const std::string& p : v) {
			pageSet.insert(p);
		}
	}

	std::string formattedPage;
	{
		std::vector<std::string> v;
		boost::split(v, page, boost::is_any_of(","));
		for (std::string& p : v) {
			boost::trim(p);
			if (!p.empty()) {
				if (pageSet.count(p) == 0) {
					return Rpc::ec_page_does_not_exist;
				}
				if (!formattedPage.empty()) {
					formattedPage += ",";
				}
				formattedPage += "(" + p + ")";
			}
		}
	}

	if (formattedPage.empty()) {
		return Rpc::ec_page_does_not_exist;
	}

	form_["Page"] = formattedPage;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setCategory(const std::string& category, const Ice::Current&)
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

Rpc::ErrorCode RpcContentSubmitterImpl::setEngine(const std::string& name, const std::string& version, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	std::list<std::string> versions;
	boost::split(versions, version, boost::is_any_of("|"));

	if (versions.empty()) {
		return Rpc::ec_engine_version_does_not_exist;
	}

	for (const std::string& v : versions)
	{
		std::string state;
		if (!context_->center()->getEngineVersionState(name, v, state)) {
			return Rpc::ec_engine_version_does_not_exist;
		}
		if (state != "Normal") {
			if (state == "Removed") {
				return Rpc::ec_engine_version_is_removed;
			}
			assert(false);
		}
	}

	form_["EngineName"] = name;
	form_["EngineVersion"] = version;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setStartup(const std::string& startup, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Startup"] = startup;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setParentId(const std::string& uid, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	std::map<std::string, std::string> form;
	if (!context_->center()->getContent(form, uid)) {
		return Rpc::ec_parent_does_not_exist;
	}

	form_["ParentId"] = uid;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setVideo(const std::string& video, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Video"] = video;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::setDescription(const std::string& desc, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	form_["Desc"] = desc;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::uploadImage(Ice::Int index, Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (index < 0) {
		return Rpc::ec_invalid_operation;
	}

	Uploader& imageUploader = imageUploaders_[index];
	if (imageUploader.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr ptr = new RpcFileUploaderImpl;

	fs::path filename = base_;
	filename /= "image_" + std::to_string(index);

	Rpc::ErrorCode ec = ptr->init(filename.string());
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(ptr));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	imageUploader.first = uploaderPrx;
	imageUploader.second = ptr;

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcContentSubmitterImpl::uploadContent(Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (mode_ == update_mode) {
		return Rpc::ec_access_denied;
	}

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (contentUploader_.first) {
		return Rpc::ec_invalid_operation;
	}

	RpcFileUploaderImplPtr ptr = new RpcFileUploaderImpl;

	fs::path filename = base_;
	filename /= "content";

	Rpc::ErrorCode ec = ptr->init(filename.string());
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(ptr));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	contentUploader_.first = uploaderPrx;
	contentUploader_.second = ptr;

	return Rpc::ec_success;
}

void RpcContentSubmitterImpl::cancel(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return;
	}

	if (contentUploader_.first) {
		contentUploader_.first->cancel();
	}

	for (auto& p : imageUploaders_) {
		p.second.first->cancel();
	}

	cancelled_ = true;
}

Rpc::ErrorCode RpcContentSubmitterImpl::finish(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (finished_ || cancelled_) {
		return Rpc::ec_invalid_operation;
	}

	if (!form_.count("Title") || !form_.count("Page") ||
		!form_.count("Category") || !form_.count("EngineName") ||
		!form_.count("EngineVersion") || !form_.count("Startup") ||
		!form_.count("Desc")) {
		return Rpc::ec_incomplete_form;
	}

	if (mode_ == submit_mode)
	{
		int imageCount = 0;

		for (int i = 0; i <= std::numeric_limits<int>::max(); ++i) {
			if (imageUploaders_.count(i) && imageUploaders_[i].second && imageUploaders_[i].second->isFinished()) {
				++imageCount;
			}
			else {
				break;
			}
		}

		if (imageCount < 1) {
			return Rpc::ec_incomplete_form;
		}

		form_["ImageCount"] = std::to_string(imageCount);

		if (!contentUploader_.second || !contentUploader_.second->isFinished()) {
			return Rpc::ec_incomplete_content;
		}

		form_["User"] = context_->user();

		setEmptyIfNotExist(form_, "ParentId");
		setEmptyIfNotExist(form_, "Video");
		context_->center()->addContent(form_, id_);
	}
	else
	{
		setEmptyIfNotExist(form_, "ParentId");
		setEmptyIfNotExist(form_, "Video");
		context_->center()->updateContent(form_, id_);
	}

	return Rpc::ec_success;
}

void RpcContentSubmitterImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

