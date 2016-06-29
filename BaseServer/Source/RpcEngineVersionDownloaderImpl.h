#ifndef BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_
#define BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_

#include "Center.h"
#include "RpcFileDownloaderImpl.h"

class RpcEngineVersionDownloaderImpl : public RpcFileDownloaderImpl {
public:
	explicit RpcEngineVersionDownloaderImpl(CenterPtr);
	~RpcEngineVersionDownloaderImpl();

	Rpc::ErrorCode init(const std::string& name, const std::string& version);

	virtual void finish(const Ice::Current&);
	virtual void cancel(const Ice::Current&);

private:
	void unlockEngineVersionIfLocked();

private:
	CenterPtr center_;
	bool EngineVerLocked_;
	std::string name_;
	std::string version_;
};

typedef IceUtil::Handle<RpcEngineVersionDownloaderImpl> RpcEngineDownloaderImplPtr;

#endif // BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_

