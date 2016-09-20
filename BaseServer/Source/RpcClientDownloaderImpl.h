#ifndef BASESERVER_RPCCLIENTDOWNLOADERIMPL_HEADER_
#define BASESERVER_RPCCLIENTDOWNLOADERIMPL_HEADER_

#include "Center.h"
#include "RpcFileDownloaderImpl.h"

#include <IceUtil/IceUtil.h>

class RpcClientDownloaderImpl : public RpcFileDownloaderImpl {
public:
	explicit RpcClientDownloaderImpl(CenterPtr);
	~RpcClientDownloaderImpl();

	Rpc::ErrorCode init(const std::string& version);

	virtual Rpc::ErrorCode read(Ice::Long, Ice::Int, Rpc::ByteSeq&, const Ice::Current&);
	virtual void finish(const Ice::Current&);
	virtual void cancel(const Ice::Current&);

	IceUtil::Time timestamp();

private:
	void unlockClientVersionIfLocked();

private:
	CenterPtr center_;
	IceUtil::Time timestamp_;
	bool clientVerLocked_;
	std::string version_;
};

typedef IceUtil::Handle<RpcClientDownloaderImpl> RpcClientDownloaderImplPtr;

#endif // BASESERVER_RPCCLIENTDOWNLOADERIMPL_HEADER_

