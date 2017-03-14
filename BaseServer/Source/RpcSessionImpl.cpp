#include "RpcSessionImpl.h"
#include "RpcEngineVersionBrowserImpl.h"
#include "RpcEngineVersionSubmitterImpl.h"
#include "RpcEngineVersionDownloaderImpl.h"
#include "RpcContentSubmitterImpl.h"
#include "RpcContentBrowserImpl.h"
#include "RpcExtraBrowserImpl.h"
#include "RpcExtraSubmitterImpl.h"
#include "RpcClientBrowserImpl.h"
#include "RpcClientSubmitterImpl.h"
#include "RpcUserBrowserImpl.h"
#include "PathUtils.h"

#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>

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

Rpc::ErrorCode RpcSessionImpl::setContentCategories(const Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	context_->center()->setContentCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getContentCategories(Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	context_->center()->getContentCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::setExtraCategories(const Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	context_->center()->setExtraCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getExtraCategories(Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	context_->center()->getExtraCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseContent(const std::string& page, const std::string& category, const std::string& search, Rpc::ContentBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcContentBrowserImplPtr browser = new RpcContentBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init(page, category, search);
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

	std::string page = form.at("Page");
	page.erase(std::remove_if(page.begin(), page.end(), boost::is_any_of("()")), page.end());
	info.page = page;

	std::string category = form.at("Category");
	category.erase(std::remove_if(category.begin(), category.end(), boost::is_any_of("()")), category.end());
	info.category = category;

	info.engineName = form.at("EngineName");
	info.engineVersion = form.at("EngineVersion");
	info.startup = form.at("Startup");
	info.imageCount = std::stoi(form.at("ImageCount"));
	info.video = form.at("Video");
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

Rpc::ErrorCode RpcSessionImpl::browseEngineVersions(bool all, Rpc::EngineVersionBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineBrowserImplPtr browser = new RpcEngineVersionBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init(all);
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

	std::string safeFilename = makeSafePath(context_->center()->getEnginePath(name, version));

	boost::system::error_code ec;
	fs::remove(safeFilename, ec);

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::submitEngineVersion(const std::string& name, const std::string& version, Rpc::EngineVersionSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcEngineVersionSubmitterImplPtr submitter = new RpcEngineVersionSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(name, version, RpcEngineVersionSubmitterImpl::submit_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::EngineVersionSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::updateEngineVersion(const std::string& name, const std::string& version, Rpc::EngineVersionSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcEngineVersionSubmitterImplPtr submitter = new RpcEngineVersionSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(name, version, RpcEngineVersionSubmitterImpl::update_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::EngineVersionSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getEngineVersion(const std::string& name, const std::string& version, Rpc::EngineVersionInfo& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	Form form;
	if (!context_->center()->getEngineVersion(name, version, form)) {
		return Rpc::ec_engine_version_does_not_exist;
	}

	info.name = name;
	info.version = version;
	info.setup = form.at("Setup");
	info.unsetup = form.at("UnSetup");
	info.uptime = form.at("UpTime");
	info.info = form.at("Info");
	info.state = form.at("State");

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseExtra(const std::string& category, const std::string& search, Rpc::ExtraBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcExtraBrowserImplPtr browser = new RpcExtraBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init(category, search);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::ExtraBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	if (!context_->objectManager()->addObject(browserPrx)) {
		browserPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}


Rpc::ErrorCode RpcSessionImpl::getExtraInfo(const std::string& id, Rpc::ExtraInfo& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getExtra(form, id)) {
		return Rpc::ec_extra_does_not_exist;
	}

	info.id = form.at("Id");
	info.title = form.at("Title");

	std::string category = form.at("Category");
	category.erase(std::remove_if(category.begin(), category.end(), boost::is_any_of("()")), category.end());
	info.category = category;

	info.setup = form.at("Setup");
	info.user = form.at("User");
	info.uptime = form.at("UpTime");
	info.info = form.at("Info");
	info.state = form.at("State");

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::downloadExtraImage(const std::string& id, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getExtra(form, id)) {
		return Rpc::ec_extra_does_not_exist;
	}

	RpcFileDownloaderImplPtr downloader = new RpcFileDownloaderImpl;

	fs::path path = context_->center()->getExtraPath(id);
	path /= "image_0";

	Rpc::ErrorCode ec = downloader->init(path.string());
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

Rpc::ErrorCode RpcSessionImpl::downloadExtra(const std::string& id, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getExtra(form, id)) {
		return Rpc::ec_extra_does_not_exist;
	}

	RpcFileDownloaderImplPtr downloader = new RpcFileDownloaderImpl;

	fs::path path = context_->center()->getExtraPath(id);
	path /= "extra";

	Rpc::ErrorCode ec = downloader->init(path.string());
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

Rpc::ErrorCode RpcSessionImpl::submitExtra(Rpc::ExtraSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcExtraSubmitterImplPtr submitter = new RpcExtraSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(RpcExtraSubmitterImpl::submit_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ExtraSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::updateExtra(const std::string& id, Rpc::ExtraSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcExtraSubmitterImplPtr submitter = new RpcExtraSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(RpcExtraSubmitterImpl::update_mode, id);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ExtraSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeExtra(const std::string& id, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (!context_->center()->changeExtraState(id, "Removed")) {
		return Rpc::ec_operation_failed;
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

Rpc::ErrorCode RpcSessionImpl::browseClient(Rpc::ClientBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcClientBrowserImplPtr browser = new RpcClientBrowserImpl(context_->center());

	Rpc::ErrorCode ec = browser->init();
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::ClientBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	if (!context_->objectManager()->addObject(browserPrx)) {
		browserPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getClientInfo(const std::string& version, Rpc::ClientInfo& info, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	std::map<std::string, std::string> form;
	if (!context_->center()->getClientVersion(version, form)) {
		return Rpc::ec_client_version_does_not_exist;
	}

	info.version = version;
	info.uptime = form.at("UpTime");
	info.info = form.at("Info");
	info.state = form.at("State");

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::submitClient(const std::string& version, Rpc::ClientSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	std::string currentVersion = context_->center()->getNewestClientVersion();

	if (!currentVersion.empty() && !versionLess(currentVersion, version)) {
		Rpc::ec_client_version_too_low;
	}

	RpcClientSubmitterImplPtr submitter = new RpcClientSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(version, RpcClientSubmitterImpl::submit_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ClientSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::updateClient(const std::string& version, Rpc::ClientSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	RpcClientSubmitterImplPtr submitter = new RpcClientSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init(version, RpcClientSubmitterImpl::update_mode);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	submitterPrx = Rpc::ClientSubmitterPrx::uncheckedCast(c.adapter->addWithUUID(submitter));

	if (!context_->objectManager()->addObject(submitterPrx)) {
		submitterPrx->destroy();
		return Rpc::ec_server_busy;
	}

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeClient(const std::string& version, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	if (context_->userGroup() != "Admin") {
		return Rpc::ec_access_denied;
	}

	ClientVersionLockGuard clientLock(context_->center().get(), version, Center::lock_write);

	if (!clientLock.isLocked()) {
		return Rpc::ec_client_version_is_locked;
	}

	std::string state;
	if (!context_->center()->getClientVersionState(version, state)) {
		return Rpc::ec_client_version_does_not_exist;
	}

	if (state != "Normal") {
		if (state == "Removed") {
			return Rpc::ec_client_version_is_removed;
		}
		assert(false);
	}

	context_->center()->changeClientVersionState(version, "Removed");

	std::string safeFilename = makeSafePath(context_->center()->getClientPath(version));

	boost::system::error_code ec;
	fs::remove(safeFilename, ec);

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

