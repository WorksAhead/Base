#ifndef ASYNCSUBMITEXTRATASK_HEADER_
#define ASYNCSUBMITEXTRATASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncSubmitExtraTask : public ASyncTask {
public:
	ASyncSubmitExtraTask(ContextPtr context, Rpc::ExtraSubmitterPrx submitter);
	~ASyncSubmitExtraTask();

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
	Rpc::ExtraSubmitterPrx submitter_;

	std::string path_;
	std::string infoHead_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCSUBMITEXTRATASK_HEADER_

