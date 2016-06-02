#include "RpcSessionImpl.h"
#include "RpcEngineVersionBrowserImpl.h"
#include "RpcEngineVersionUploaderImpl.h"
#include "RpcEngineVersionDownloaderImpl.h"

#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

RpcSessionImpl::RpcSessionImpl(CenterPtr center)
	: destroy_(false), timestamp_(IceUtil::Time::now(IceUtil::Time::Monotonic)), center_(center)
{
}

RpcSessionImpl::~RpcSessionImpl()
{
}

void RpcSessionImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	destroy_ = true;

	try {
		c.adapter->remove(c.id);
		for (const Ice::Identity& id : ids_) {
			if (c.adapter->find(id)) {
				c.adapter->remove(id);
			}
		}
	}
	catch(const Ice::ObjectAdapterDeactivatedException&) {
	}

	ids_.clear();
}

void RpcSessionImpl::refresh(const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();
	timestamp_ = IceUtil::Time::now(IceUtil::Time::Monotonic);
}

Rpc::ErrorCode RpcSessionImpl::setPages(const Rpc::StringSeq& pages, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	center_->setPages(pages);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getPages(Rpc::StringSeq& pages, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	center_->getPages(pages);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::setCategories(const Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	center_->setCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::getCategories(Rpc::StringSeq& categories, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	center_->getCategories(categories);
	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::browseContent(const ::std::string& page, const ::std::string& category, const ::std::string& orderBy, Rpc::ContentBrowserPrx& browserPrx, const Ice::Current& c)
{
	return Rpc::ec_access_denied;
}

Rpc::ErrorCode RpcSessionImpl::browseEngineVersions(Rpc::EngineVersionBrowserPrx& browserPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineBrowserImplPtr browser = new RpcEngineVersionBrowserImpl(center_);

	Rpc::ErrorCode ec = browser->init();
	if (ec != Rpc::ec_success) {
		return ec;
	}

	browserPrx = Rpc::EngineVersionBrowserPrx::uncheckedCast(c.adapter->addWithUUID(browser));

	ids_.insert(browserPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::uploadEngineVersion(const std::string& name, const std::string& version, const std::string& info, Rpc::UploaderPrx& uploaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineUploaderImplPtr uploader = new RpcEngineVersionUploaderImpl(center_);

	Rpc::ErrorCode ec = uploader->init(name, version, info);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	uploaderPrx = Rpc::UploaderPrx::uncheckedCast(c.adapter->addWithUUID(uploader));

	ids_.insert(uploaderPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::downloadEngineVersion(const std::string& name, const std::string& version, Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	RpcEngineDownloaderImplPtr downloader = new RpcEngineVersionDownloaderImpl(center_);

	Rpc::ErrorCode ec = downloader->init(name, version);
	if (ec != Rpc::ec_success) {
		return ec;
	}

	downloaderPrx = Rpc::DownloaderPrx::uncheckedCast(c.adapter->addWithUUID(downloader));

	ids_.insert(downloaderPrx->ice_getIdentity());

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcSessionImpl::removeEngineVersion(const std::string& name, const std::string& version, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	EngineVersionLockGuard engineLock(center_.get(), name, version, Center::lock_write);

	if (!engineLock.isLocked()) {
		return Rpc::ec_engine_version_is_locked;
	}

	std::string state;
	if (!center_->getEngineVersionState(name, version, state)) {
		return Rpc::ec_engine_version_does_not_exist;
	}

	if (state != "Normal") {
		if (state == "Removed") {
			return Rpc::ec_engine_version_is_removed;
		}
		assert(false);
	}

	center_->changeEngineVersionState(name, version, "Removed");

	const std::string& filename = center_->engineFileName(name, version);

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
	if (destroy_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

