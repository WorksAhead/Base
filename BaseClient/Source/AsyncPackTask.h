#ifndef ASYNCPACKTASK_HEADER_
#define ASYNCPACKTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncPackTask : public ASyncTask {
public:
	explicit ASyncPackTask(ContextPtr context);
	~ASyncPackTask();

	void setInfoHead(const std::string&);
	void setPath(const std::string&);

	std::string package();

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
	std::string package_;
	std::string infoHead_;
	std::string infoBody_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCPACKTASK_HEADER_

