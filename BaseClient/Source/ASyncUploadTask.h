#ifndef ASYNCUPLOADTASK_HEADER_
#define ASYNCUPLOADTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncUploadTask : public ASyncTask {
public:
	ASyncUploadTask(ContextPtr context, Rpc::UploaderPrx uploader);
	~ASyncUploadTask();

	void setInfoHead(const std::string&);
	void setFilename(const std::string&);

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();
	bool checkCancelled();

private:
	ContextPtr context_;
	Rpc::UploaderPrx uploader_;

	std::string filename_;
	std::string infoHead_;
	std::string infoBody_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCUPLOADTASK_HEADER_

