#include "RpcSessionImpl.h"
#include "RpcEngineVersionBrowserImpl.h"
#include "RpcEngineVersionUploaderImpl.h"
#include "RpcEngineVersionDownloaderImpl.h"
#include "RpcContentSubmitterImpl.h"

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

Rpc::ErrorCode RpcSessionImpl::setPages(const Rpc::StringSeq& pages, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

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

Rpc::ErrorCode RpcSessionImpl::browseContent(const ::std::string& page, const ::std::string& category, const ::std::string& orderBy, Rpc::ContentBrowserPrx& browserPrx, const Ice::Current& c)
{
	return Rpc::ec_access_denied;
}

Rpc::ErrorCode RpcSessionImpl::submitContent(Rpc::ContentSubmitterPrx& submitterPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcContentSubmitterImplPtr submitter = new RpcContentSubmitterImpl(context_);

	Rpc::ErrorCode ec = submitter->init();
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

Rpc::ErrorCode RpcSessionImpl::uploadEngineVersion(const std::string& name, const std::string& version, const std::string& info, Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

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

	if (!fs::remove(filename)) {
		return Rpc::ec_file_io_error;
	}

	return Rpc::ec_success;
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

