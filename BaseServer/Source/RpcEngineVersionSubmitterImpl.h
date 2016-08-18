#ifndef BASESERVER_RPCENGINEVERSIONSUBMITTERIMPL_HEADER_
#define BASESERVER_RPCENGINEVERSIONSUBMITTERIMPL_HEADER_

#include "Context.h"
#include "RpcFileUploaderImpl.h"

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>

class RpcEngineVersionSubmitterImpl : public Rpc::EngineVersionSubmitter {
private:
	typedef std::pair<Rpc::UploaderPrx, RpcFileUploaderImplPtr> Uploader;

public:
	enum {
		submit_mode,
		update_mode,
	};

public:
	explicit RpcEngineVersionSubmitterImpl(ContextPtr);
	~RpcEngineVersionSubmitterImpl();

	Rpc::ErrorCode init(const std::string& name, const std::string& version, int mode);
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode setSetup(const std::string&, const ::Ice::Current&);

	virtual Rpc::ErrorCode setUnSetup(const std::string&, const ::Ice::Current&);

	virtual Rpc::ErrorCode setInfo(const std::string&, const ::Ice::Current&);

	virtual Rpc::ErrorCode uploadEngine(Rpc::UploaderPrx&, const ::Ice::Current&);

	virtual void cancel(const ::Ice::Current&);

	virtual Rpc::ErrorCode finish(const Ice::Current&);

private:
	void checkIsDestroyed();
	void unlockEngineVersionIfLocked();

public:
	ContextPtr context_;
	bool EngineVerLocked_;
	bool destroyed_;
	bool finished_;
	bool cancelled_;
	std::string name_;
	std::string version_;
	int mode_;
	Uploader engineUploader_;
	std::map<std::string, std::string> form_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcEngineVersionSubmitterImpl> RpcEngineVersionSubmitterImplPtr;

#endif // BASESERVER_RPCENGINEVERSIONSUBMITTERIMPL_HEADER_

