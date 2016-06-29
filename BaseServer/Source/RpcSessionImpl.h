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

	virtual Rpc::ErrorCode setPages(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getPages(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode setCategories(const Rpc::StringSeq&, const Ice::Current&);
	virtual Rpc::ErrorCode getCategories(Rpc::StringSeq&, const Ice::Current&);

	virtual Rpc::ErrorCode browseContent(const ::std::string&, const ::std::string&, const ::std::string&, Rpc::ContentBrowserPrx&, const Ice::Current&);

	virtual Rpc::ErrorCode submitContent(Rpc::ContentSubmitterPrx&, const Ice::Current&);

	virtual Rpc::ErrorCode browseEngineVersions(Rpc::EngineVersionBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode uploadEngineVersion(const std::string&, const std::string&, const std::string&, Rpc::UploaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadEngineVersion(const std::string&, const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeEngineVersion(const std::string&, const std::string&, const Ice::Current&);

	IceUtil::Time timestamp();

private:
	void checkIsDestroyed();

private:
	bool destroyed_;
	IceUtil::Time timestamp_;
	ContextPtr context_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcSessionImpl> RpcSessionImplPtr;

#endif // BASESERVER_RPCSESSIONIMPL_HEADER_

