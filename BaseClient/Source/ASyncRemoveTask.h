#ifndef ASYNCREMOVETASK_HEADER_
#define ASYNCREMOVETASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncRemoveTask : public ASyncTask {
public:
	ASyncRemoveTask();
	~ASyncRemoveTask();

	void setInfoHead(const std::string&);
	void setPath(const std::string&);

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();

private:
	std::string path_;
	std::string infoHead_;
	std::string infoBody_;

	int state_;
	int progress_;
	bool cancelled_;

	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCREMOVETASK_HEADER_

