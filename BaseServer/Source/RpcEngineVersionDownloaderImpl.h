#ifndef BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_
#define BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_

#include "Center.h"

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>

class RpcEngineVersionDownloaderImpl : public Rpc::Downloader {
public:
	explicit RpcEngineVersionDownloaderImpl(CenterPtr);
	~RpcEngineVersionDownloaderImpl();

	Rpc::ErrorCode init(const std::string& name, const std::string& version);

	virtual Rpc::ErrorCode getSize(Ice::Long&, const Ice::Current&);

	virtual Rpc::ErrorCode read(Ice::Long, Ice::Int, Rpc::ByteSeq&, const Ice::Current&);
	virtual void finish(const Ice::Current&);
	virtual void cancel(const Ice::Current&);

private:
	CenterPtr center_;
	bool EngineVerLocked_;
	std::string name_;
	std::string version_;
	boost::shared_ptr<std::fstream> stream_;
	std::streamsize size_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcEngineVersionDownloaderImpl> RpcEngineDownloaderImplPtr;

#endif // BASESERVER_RPCENGINEDOWNLOADERIMPL_HEADER_

