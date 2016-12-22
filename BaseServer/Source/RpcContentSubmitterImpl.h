#ifndef BASESERVER_RPCCONTENTSUBMITTERIMPL_HEADER_
#define BASESERVER_RPCCONTENTSUBMITTERIMPL_HEADER_

#include "Context.h"
#include "RpcFileUploaderImpl.h"

#include <RpcSession.h>

#include <boost/thread/recursive_mutex.hpp>

#include <map>
#include <utility>

class RpcContentSubmitterImpl : public Rpc::ContentSubmitter {
private:
	typedef std::pair<Rpc::UploaderPrx, RpcFileUploaderImplPtr> Uploader;

public:
	enum {
		submit_mode,
		update_mode,
	};

public:
	explicit RpcContentSubmitterImpl(ContextPtr);
	~RpcContentSubmitterImpl();

	Rpc::ErrorCode init(int mode = submit_mode, const std::string& id = "");
	bool isFinished();
	bool isCancelled();

	virtual void destroy(const Ice::Current&);

	virtual Rpc::ErrorCode setTitle(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setPage(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setCategory(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setEngine(const std::string&, const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setStartup(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setParentId(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setVideo(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode setDescription(const std::string&, const Ice::Current&);

	virtual Rpc::ErrorCode uploadImage(Ice::Int, Rpc::UploaderPrx&, const Ice::Current&);

	virtual Rpc::ErrorCode uploadContent(Rpc::UploaderPrx&, const Ice::Current&);

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
	Uploader contentUploader_;
	std::map<Ice::Int, Uploader> imageUploaders_;
	std::map<std::string, std::string> form_;
	boost::recursive_mutex sync_;
};

typedef IceUtil::Handle<RpcContentSubmitterImpl> RpcContentSubmitterImplPtr;

#endif // BASESERVER_RPCCONTENTSUBMITTERIMPL_HEADER_

