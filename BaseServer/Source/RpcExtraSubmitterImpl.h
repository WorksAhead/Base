#ifndef BASESERVER_RPCEXTRASUBMITTERIMPL_HEADER_
#define BASESERVER_RPCEXTRASUBMITTERIMPL_HEADER_

#include "Context.h"
#include "RpcFileUploaderImpl.h"

#include <boost/thread/recursive_mutex.hpp>

#include <map>
#include <utility>

class RpcExtraSubmitterImpl : public Rpc::ExtraSubmitter {
private:
	typedef std::pair<Rpc::UploaderPrx, RpcFileUploaderImplPtr> Uploader;

public:
	enum {
		submit_mode,
		update_mode,
	};

public:
	explicit RpcExtraSubmitterImpl(ContextPtr);
	~RpcExtraSubmitterImpl();

	Rpc::ErrorCode init(int mode = submit_mode, const std::string& id = "");
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode setTitle(const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode setSetup(const std::string&, const Ice::Current&);
	virtual Rpc::ErrorCode setInfo(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode uploadExtra(Rpc::UploaderPrx&, const Ice::Current&);

	virtual void cancel(const Ice::Current&);

	virtual Rpc::ErrorCode finish(const Ice::Current&);

private:
	void checkIsDestroyed();

public:
	ContextPtr context_;
	bool destroyed_;
	bool finished_;
	bool cancelled_;
	std::string id_;
	std::string base_;
	int mode_;
	Uploader uploader_;
	std::map<std::string, std::string> form_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcExtraSubmitterImpl> RpcExtraSubmitterImplPtr;

#endif // BASESERVER_RPCEXTRASUBMITTERIMPL_HEADER_

