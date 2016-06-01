#ifndef UPLOADTASK_HEADER_
#define UPLOADTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class UploadTask : public ASyncTask {
public:
	UploadTask(ContextPtr context, std::string infoHead, const std::string& path, Rpc::UploaderPrx uploader);
	~UploadTask();

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();

private:
	ContextPtr context_;
	std::string path_;
	std::string infoHead_;
	std::string infoBody_;
	Rpc::UploaderPrx uploader_;

	int state_;
	int progress_;
	bool cancel_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // UPLOADTASK_HEADER_

