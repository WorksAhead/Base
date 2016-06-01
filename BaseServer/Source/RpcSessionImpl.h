#ifndef BASESERVER_RPCSESSIONIMPL_HEADER_
#define BASESERVER_RPCSESSIONIMPL_HEADER_

#include "Center.h"

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>

#include <boost/thread/recursive_mutex.hpp>

#include <set>

class RpcSessionImpl : public Rpc::Session {
public:
	explicit RpcSessionImpl(CenterPtr);
	~RpcSessionImpl();

	virtual void destroy(const Ice::Current&);
	virtual void refresh(const Ice::Current&);

	virtual Rpc::ErrorCode browseEngineVersions(Rpc::EngineVersionBrowserPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode uploadEngineVersion(const std::string&, const std::string&, const std::string&, Rpc::UploaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode downloadEngineVersion(const std::string&, const std::string&, Rpc::DownloaderPrx&, const Ice::Current&);
	virtual Rpc::ErrorCode removeEngineVersion(const std::string&, const std::string&, const Ice::Current&);

	IceUtil::Time timestamp();

private:
	void checkIsDestroyed();

private:
	bool destroy_;
	IceUtil::Time timestamp_;
	CenterPtr center_;
	boost::recursive_mutex sync_;
	std::set<Ice::Identity> ids_;
};

typedef IceUtil::Handle<RpcSessionImpl> RpcSessionImplPtr;

#endif // BASESERVER_RPCSESSIONIMPL_HEADER_

