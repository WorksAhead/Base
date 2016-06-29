#ifndef BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_
#define BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_

#include "Center.h"
#include "RpcFileUploaderImpl.h"

class RpcEngineVersionUploaderImpl : public RpcFileUploaderImpl {
public:
	explicit RpcEngineVersionUploaderImpl(CenterPtr);
	~RpcEngineVersionUploaderImpl();

	Rpc::ErrorCode init(const std::string& name, const std::string& version, const std::string& info);

	virtual Rpc::ErrorCode finish(Ice::Int, const Ice::Current&);
	virtual void cancel(const Ice::Current&);

private:
	void unlockEngineVersionIfLocked();

private:
	CenterPtr center_;
	bool EngineVerLocked_;
	std::string name_;
	std::string version_;
	std::string info_;
};

typedef IceUtil::Handle<RpcEngineVersionUploaderImpl> RpcEngineUploaderImplPtr;

#endif // BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_

