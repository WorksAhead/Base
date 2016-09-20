#ifndef BASESERVER_RPCCLIENTSUBMITTERIMPL_HEADER_
#define BASESERVER_RPCCLIENTSUBMITTERIMPL_HEADER_

#include "Context.h"
#include "RpcFileUploaderImpl.h"

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>

class RpcClientSubmitterImpl : public Rpc::ClientSubmitter {
private:
	typedef std::pair<Rpc::UploaderPrx, RpcFileUploaderImplPtr> Uploader;

public:
	enum {
		submit_mode,
		update_mode,
	};

public:
	explicit RpcClientSubmitterImpl(ContextPtr);
	~RpcClientSubmitterImpl();

	Rpc::ErrorCode init(const std::string& version, int mode);
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode setInfo(const std::string&, const ::Ice::Current&);

	virtual Rpc::ErrorCode uploadClient(Rpc::UploaderPrx&, const ::Ice::Current&);

	virtual void cancel(const ::Ice::Current&);

	virtual Rpc::ErrorCode finish(const Ice::Current&);

private:
	void checkIsDestroyed();
	void unlockClientVersionIfLocked();

public:
	ContextPtr context_;
	bool clientVerLocked_;
	bool destroyed_;
	bool finished_;
	bool cancelled_;
	std::string version_;
	int mode_;
	Uploader clientUploader_;
	std::map<std::string, std::string> form_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcClientSubmitterImpl> RpcClientSubmitterImplPtr;

#endif // BASESERVER_RPCCLIENTSUBMITTERIMPL_HEADER_

