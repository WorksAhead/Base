#ifndef ASYNCSUBMITCLIENTTASK_HEADER_
#define ASYNCSUBMITCLIENTTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncSubmitClientTask : public ASyncTask {
public:
	ASyncSubmitClientTask(ContextPtr context, Rpc::ClientSubmitterPrx submitter);
	~ASyncSubmitClientTask();

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
	Rpc::ClientSubmitterPrx submitter_;

	std::string path_;
	std::string infoHead_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCSUBMITCLIENTTASK_HEADER_

