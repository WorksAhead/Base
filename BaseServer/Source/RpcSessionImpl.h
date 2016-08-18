#ifndef BASESERVER_RPCSESSIONIMPL_HEADER_
#define BASESERVER_RPCSESSIONIMPL_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>

#include <boost/thread/recursive_mutex.hpp>

#include <set>

class RpcSessionImpl : public Rpc::Session {
public:
	explicit RpcSessionImpl(ContextPtr);
	~RpcSessionImpl();

	virtual void destroy(const Ice::Current&);

	virtual void refresh(const Ice::Current&);

	virtual Rpc::ErrorCode getCurrentUser(std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode getCurrentUserGroup(std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setPages(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getPages(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode setCategories(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getCategories(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode browseContent(const std::string&, const std::string&, Rpc::ContentBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getContentInfo(const std::string&, Rpc::ContentInfo&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadContentImage(const std::string&, Ice::Int, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadContent(const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode submitContent(Rpc::ContentSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode updateContent(const std::string&, Rpc::ContentSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeContent(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode browseEngineVersions(Rpc::EngineVersionBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadEngineVersion(const std::string&, const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeEngineVersion(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode submitEngineVersion(const std::string&, const std::string&, Rpc::EngineVersionSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode updateEngineVersion(const std::string&, const std::string&, Rpc::EngineVersionSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getEngineVersion(const std::string&, const std::string&, Rpc::EngineVersion&, const Ice::Current&);

	virtual Rpc::ErrorCode browseUsers(Rpc::UserBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode setUserGroup(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode resetUserPassword(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode removeUser(const std::string&, const Ice::Current&);

	IceUtil::Time timestamp();

private:
	void checkIsDestroyed();
	Rpc::ErrorCode downloadContentFile(const std::string& id, const std::string& path, Rpc::DownloaderPrx&, const Ice::Current&);

private:
	bool destroyed_;
	IceUtil::Time timestamp_;
	ContextPtr context_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcSessionImpl> RpcSessionImplPtr;

#endif // BASESERVER_RPCSESSIONIMPL_HEADER_

