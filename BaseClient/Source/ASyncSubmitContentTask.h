#ifndef ASYNCSUBMITCONTENTTASK_HEADER_
#define ASYNCSUBMITCONTENTTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <vector>
#include <memory>
#include <thread>
#include <string>

class ASyncSubmitContentTask : public ASyncTask {
public:
	ASyncSubmitContentTask(ContextPtr context, Rpc::ContentSubmitterPrx);
	~ASyncSubmitContentTask();

	void setInfoHead(const std::string&);

	void setContentLocation(const std::string&);
	void addImageFile(const std::string&);

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
	Rpc::ContentSubmitterPrx submitter_;

	std::string infoHead_;
	std::string info_;
	std::string contentLocation_;
	std::vector<std::string> imageFiles_;

	int state_;
	int progress_;
	bool cancelled_;

	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCSUBMITCONTENTTASK_HEADER_

