#ifndef BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_
#define BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_

#include "Center.h"

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>

class RpcEngineUploaderImpl : public Rpc::EngineUploader {
public:
	explicit RpcEngineUploaderImpl(CenterPtr);
	~RpcEngineUploaderImpl();

	Rpc::ErrorCode init(const std::string& name, const std::string& version, const std::string& info);

	virtual Rpc::ErrorCode write(Ice::Long, const std::pair<const Ice::Byte*, const Ice::Byte*>&, const Ice::Current&);
	virtual Rpc::ErrorCode finish(Ice::Int, const Ice::Current&);

private:
	CenterPtr center_;
	bool EngineVerLocked_;
	std::string name_;
	std::string version_;
	std::string info_;
	boost::shared_ptr<std::fstream> stream_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcEngineUploaderImpl> RpcEngineUploaderImplPtr;

#endif // BASESERVER_RPCENGINEUPLOADERIMPL_HEADER_

