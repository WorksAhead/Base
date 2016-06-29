#ifndef BASESERVER_RPCFILEUPLOADERIMPL_HEADER_
#define BASESERVER_RPCFILEUPLOADERIMPL_HEADER_

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>

class RpcFileUploaderImpl : public Rpc::Uploader {
public:
	RpcFileUploaderImpl();
	~RpcFileUploaderImpl();

	Rpc::ErrorCode init(const std::string& filename);

	const std::string& filename();
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode write(Ice::Long, const std::pair<const Ice::Byte*, const Ice::Byte*>&, const Ice::Current&);
	virtual Rpc::ErrorCode finish(Ice::Int, const Ice::Current&);
	virtual void cancel(const Ice::Current&);

protected:
	void checkIsDestroyed();

protected:
	bool destroyed_;
	bool finished_;
	bool cancelled_;
	std::string filename_;
	boost::shared_ptr<std::fstream> stream_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcFileUploaderImpl> RpcFileUploaderImplPtr;

#endif // BASESERVER_RPCFILEUPLOADERIMPL_HEADER_

