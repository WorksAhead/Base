#ifndef BASESERVER_RPCFILEDOWNLOADERIMPL_HEADER_
#define BASESERVER_RPCFILEDOWNLOADERIMPL_HEADER_

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>

class RpcFileDownloaderImpl : public Rpc::Downloader {
public:
	RpcFileDownloaderImpl();
	~RpcFileDownloaderImpl();

	Rpc::ErrorCode init(const std::string& filename);

	const std::string& filename();
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode getSize(Ice::Long&, const Ice::Current&);

	virtual Rpc::ErrorCode read(Ice::Long, Ice::Int, Rpc::ByteSeq&, const Ice::Current&);
	virtual void finish(const Ice::Current&);
	virtual void cancel(const Ice::Current&);

protected:
	void checkIsDestroyed();

protected:
	bool destroyed_;
	bool finished_;
	bool cancelled_;
	std::string filename_;
	boost::shared_ptr<std::fstream> stream_;
	std::streamsize size_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcFileDownloaderImpl> RpcFileDownloaderImplPtr;

#endif // BASESERVER_RPCFILEDOWNLOADERIMPL_HEADER_

