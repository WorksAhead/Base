#ifndef BASESERVER_RPCSTARTIMPL_HEADER_
#define BASESERVER_RPCSTARTIMPL_HEADER_

#include "Center.h"
#include "RpcSessionMaintainer.h"

#include <RpcStart.h>

#include "Security/Rijndael.h"

class RpcStartImpl : public Rpc::Start {
public:
	RpcStartImpl(CenterPtr);

	virtual std::string getServerVersion(const Ice::Current&);

	virtual std::string getClientVersion(const Ice::Current&);

	virtual Rpc::ErrorCode downloadClient(Rpc::DownloaderPrx& downloader, const Ice::Current&);

	virtual Rpc::ErrorCode login(const std::string&, const std::string&, Rpc::SessionPrx&, const Ice::Current&);

private:
	bool loginToCYou(const std::string&, const std::string&);

private:
	CenterPtr center_;
	IceUtil::TimerPtr timer_;
	RpcSessionMaintainerPtr maintainer_;
	CRijndael rijndael_;
};

#endif // BASESERVER_RPCSTARTIMPL_HEADER_

