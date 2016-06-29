#ifndef ASYNCSUBMITENGINETASK_HEADER_
#define ASYNCSUBMITENGINETASK_HEADER_

#include "ASyncTask.h"
#include "AsyncPackTask.h"
#include "ASyncUploadTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncSubmitEngineTask : public ASyncTask {
public:
	ASyncSubmitEngineTask(ContextPtr context, Rpc::UploaderPrx uploader);
	~ASyncSubmitEngineTask();

	void setInfoHead(const std::string&);

	void setPath(const std::string&);

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();
	int update(ASyncTask*, int, double);

private:
	ContextPtr context_;
	Rpc::UploaderPrx uploader_;

	std::string path_;
	std::string infoHead_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCSUBMITENGINETASK_HEADER_

