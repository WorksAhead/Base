#ifndef BASESERVER_RPCSTARTIMPL_HEADER_
#define BASESERVER_RPCSTARTIMPL_HEADER_

#include "Center.h"
#include "RpcSessionMaintainer.h"

#include <RpcStart.h>

class RpcStartImpl : public Rpc::Start {
public:
	RpcStartImpl(CenterPtr);

	virtual std::string getServerVersion(const Ice::Current&);

	virtual Rpc::ErrorCode signup(const std::string&, const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode login(const std::string&, const std::string&, Rpc::SessionPrx&, const Ice::Current&);

	virtual Rpc::ErrorCode resetPassword(const std::string&, const std::string&, const std::string&, const Ice::Current&);

private:
	CenterPtr center_;
	IceUtil::TimerPtr timer_;
	RpcSessionMaintainerPtr maintainer_;
};

#endif // BASESERVER_RPCSTARTIMPL_HEADER_

