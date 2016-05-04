#ifndef BASESERVER_RPCSTARTIMPL_HEADER_
#define BASESERVER_RPCSTARTIMPL_HEADER_

#include "SQLiteUtil.h"
#include "RpcSessionMaintainer.h"

#include <RpcStart.h>

class RpcStartImpl : public Rpc::Start {
public:
	RpcStartImpl(RpcSessionMaintainerPtr, DatabasePtr);

	virtual std::string getServerVersion(const Ice::Current&);

	virtual Rpc::ErrorCode signup(const std::string& username, const std::string& password, const Ice::Current&);

	virtual Rpc::ErrorCode login(const std::string& username, const std::string& password, Rpc::SessionPrx& sessionPrx, const Ice::Current&);

private:
	RpcSessionMaintainerPtr maintainer_;
	DatabasePtr db_;
};

#endif // BASESERVER_RPCSTARTIMPL_HEADER_

