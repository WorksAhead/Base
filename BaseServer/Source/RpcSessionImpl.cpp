#include "RpcSessionImpl.h"
#include "RpcEngineVersionBrowserImpl.h"
#include "RpcEngineVersionUploaderImpl.h"
#include "RpcEngineVersionDownloaderImpl.h"
#include "RpcContentSubmitterImpl.h"
#include "RpcContentBrowserImpl.h"
#include "RpcUserBrowserImpl.h"

#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

RpcSessionImpl::RpcSessionImpl(ContextPtr context)
	: destroyed_(false), timestamp_(IceUtil::Time::now(IceUtil::Time::Monotonic)), context_(context)
{
}

RpcSessionImpl::~RpcSessionImpl()
{
}

void RpcSessionImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	destroyed_ = true;

	try {
		context_->objectManager()->destroyAllObjects(c);
		c.adapter->remove(c.id);
	}
	catch (const Ice::ObjectAdapterDeactivatedException&) {
	}
}

void RpcSessionImpl::refresh(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	context_->objectManager()->refresh(c);

	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
}

Rpc::ErrorCode RpcSessionImpl::getCurrentUser(std::string& outUser, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	outUser = context_->user();
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getCurrentUserGroup(std::string& outGroup, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	outGroup = context_->userGroup();
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::setPages(const Rpc::StringSeq& pages, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	context_->center()->setPages(pages);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getPages(Rpc::StringSeq& pages, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	context_->center()->getPages(pages);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::setCategories(const Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	context_->center()->setCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getCategories(Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	context_->center()->getCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseContent(const std::string& page, const std::string& category, Rpc::ContentBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcContentBrowserImplPtr browser = new RpcContentBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init(page, category);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::ContentBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	if (!context_->objectManager()->addObject(browserPrx)) {
		browserPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getContentInfo(const std::string& id, Rpc::ContentInfo& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getContent(form, id)) {
		return Rpc::ec_content_does_not_exist;
	}

	info.id = form.at("Id");
	info.parentId = form.at("ParentId");
	info.title = form.at("Title");
	info.page = form.at("Page");
	info.category = form.at("Category");
	info.engineName = form.at("EngineName");
	info.engineVersion = form.at("EngineVersion");
	info.startup = form.at("Startup");
	info.imageCount = std::stoi(form.at("ImageCount"));
	info.desc = form.at("Desc");
	info.user = form.at("User");
	info.upTime = form.at("UpTime");
	info.state = form.at("State");

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::downloadContentImage(const std::string& id, Ice::Int index, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	fs::path file = context_->center()->getContentPath(id);
	file /= "image_" + std::to_string(index);

	return downloadContentFile(id, file.string(), downloaderPrx, c);
}

Rpc::ErrorCode RpcSessionImpl::downloadContent(const std::string& id, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	fs::path file = context_->center()->getContentPath(id);
	file /= "content";

	return downloadContentFile(id, file.string(), downloaderPrx, c);
}

Rpc::ErrorCode RpcSessionImpl::submitContent(Rpc::ContentSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcContentSubmitterImplPtr submitter = new RpcContentSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(RpcContentSubmitterImpl::submit_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ContentSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::updateContent(const std::string& id, Rpc::ContentSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getContent(form, id)) {
		return Rpc::ec_content_does_not_exist;
	}

	if (!boost::iequals(context_->user(), form.at("User")) && context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcContentSubmitterImplPtr submitter = new RpcContentSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(RpcContentSubmitterImpl::update_mode, id);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ContentSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeContent(const std::string& id, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (!context_->center()->changeContentState(id, "Removed")) {
		return Rpc::ec_operation_failed;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseEngineVersions(Rpc::EngineVersionBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineBrowserImplPtr browser = new RpcEngineVersionBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init();
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::EngineVersionBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	if (!context_->objectManager()->addObject(browserPrx)) {
		browserPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::downloadEngineVersion(const std::string& name, const std::string& version, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineDownloaderImplPtr downloader = new RpcEngineVersionDownloaderImpl(context_->center());

	Rpc::ErrorCode ec = downloader->init(name, version);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	downloaderPrx = Rpc::DownloaderPrx::uncheckedCast(c.adapter->addWithUUID(downloader));

	if (!context_->objectManager()->addObject(downloaderPrx)) {
		downloaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeEngineVersion(const std::string& name, const std::string& version, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	EngineVersionLockGuard engineLock(context_->center().get(), name, version, Center::lock_write);

	if (!engineLock.isLocked()) {
		return Rpc::ec_engine_version_is_locked;
	}

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

	context_->center()->changeEngineVersionState(name, version, "Removed");

	const std::string& filename = context_->center()->getEnginePath(name, version);

	boost::system::error_code ec;
	fs::remove(filename, ec);

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::submitEngineVersion(const std::string& name, const std::string& version, const std::string& info, Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcEngineUploaderImplPtr uploader = new RpcEngineVersionUploaderImpl(context_->center());

	Rpc::ErrorCode ec = uploader->init(name, version, info);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(uploader));

	if (!context_->objectManager()->addObject(uploaderPrx)) {
		uploaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseUsers(Rpc::UserBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcUserBrowserImplPtr browser = new RpcUserBrowserImpl(context_);

	Rpc::ErrorCode ec = browser->init();
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::UserBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	if (!context_->objectManager()->addObject(browserPrx)) {
		browserPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::setUserGroup(const std::string& username, const std::string& group, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	if (context_->center()->setUserGroup(username, group)) {
		return Rpc::ec_success;
	}

	return Rpc::ec_operation_failed;
}

Rpc::ErrorCode RpcSessionImpl::resetUserPassword(const std::string& username, const std::string& password, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	if (context_->center()->resetUserPassword(username, password)) {
		return Rpc::ec_success;
	}

	return Rpc::ec_operation_failed;
}

Rpc::ErrorCode RpcSessionImpl::removeUser(const std::string& username, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	if (context_->center()->removeUser(username)) {
		return Rpc::ec_success;
	}

	return Rpc::ec_operation_failed;
}

IceUtil::Time RpcSessionImpl::timestamp()
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	return timestamp_;
}

void RpcSessionImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

Rpc::ErrorCode RpcSessionImpl::downloadContentFile(const std::string& id, const std::string& path, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	std::map<std::string, std::string> form;
	if (!context_->center()->getContent(form, id)) {
		return Rpc::ec_content_does_not_exist;
	}

	RpcFileDownloaderImplPtr downloader = new RpcFileDownloaderImpl;

	Rpc::ErrorCode ec = downloader->init(path);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	downloaderPrx = Rpc::DownloaderPrx::uncheckedCast(c.adapter->addWithUUID(downloader));

	if (!context_->objectManager()->addObject(downloaderPrx)) {
		downloaderPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

