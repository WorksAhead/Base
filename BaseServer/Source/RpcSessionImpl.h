#ifndef BASESERVER_RPCSESSIONIMPL_HEADER_
#define BASESERVER_RPCSESSIONIMPL_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>

#include <boost/thread/recursive_mutex.hpp>

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

	virtual Rpc::ErrorCode setContentCategories(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getContentCategories(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode setExtraCategories(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getExtraCategories(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode setUniformInfo(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode getUniformInfo(const std::string&, std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode browseContent(const std::string&, const std::string&, const std::string&, Rpc::ContentBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode browseContentByParentId(const std::string&, Rpc::ContentBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getContentInfo(const std::string&, Rpc::ContentInfo&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadContentImage(const std::string&, Ice::Int, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadContent(const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode submitContent(Rpc::ContentSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode copyContent(const std::string&, Rpc::ContentSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode editContent(const std::string&, Rpc::ContentSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode changeContentState(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode changeContentDisplayPriority(const std::string&, Ice::Int, const Ice::Current&);

	virtual Rpc::ErrorCode browseEngineVersions(bool all, Rpc::EngineVersionBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadEngineVersion(const std::string&, const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeEngineVersion(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode submitEngineVersion(const std::string&, const std::string&, Rpc::EngineVersionSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode updateEngineVersion(const std::string&, const std::string&, Rpc::EngineVersionSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getEngineVersion(const std::string&, const std::string&, Rpc::EngineVersionInfo&, const Ice::Current&);
	virtual Rpc::ErrorCode changeEngineVersionDisplayPriority(const std::string&, const std::string&, Ice::Int, const Ice::Current&);

	virtual Rpc::ErrorCode browseExtra(const std::string&, const std::string&, Rpc::ExtraBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getExtraInfo(const std::string&, Rpc::ExtraInfo&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadExtraImage(const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadExtra(const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode submitExtra(Rpc::ExtraSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode updateExtra(const std::string&, Rpc::ExtraSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeExtra(const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode changeExtraDisplayPriority(const std::string&, Ice::Int, const Ice::Current&);

	virtual Rpc::ErrorCode browseClient(Rpc::ClientBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getClientInfo(const std::string&, Rpc::ClientInfo&, const Ice::Current&);
	virtual Rpc::ErrorCode submitClient(const std::string&, Rpc::ClientSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode updateClient(const std::string&, Rpc::ClientSubmitterPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeClient(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode browseUsers(Rpc::UserBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode setUserGroup(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode removeUser(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode browseComment(const std::string&, const std::string&, Rpc::CommentBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode getComment(const std::string&, std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode addComment(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode editComment(const std::string&, const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode removeComment(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode queryDownloadCount(const std::string&, int&, const Ice::Current&);

	virtual Rpc::ErrorCode isUserOnline(const std::string& userName, bool& result, const Ice::Current&);
	virtual Rpc::ErrorCode onlineUserCount(int& count, const Ice::Current&);

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

