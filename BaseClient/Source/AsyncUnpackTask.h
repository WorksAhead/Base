#ifndef ASYNCUNPACKTASK_HEADER_
#define ASYNCUNPACKTASK_HEADER_

#include "ASyncTask.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncUnpackTask : public ASyncTask {
public:
	ASyncUnpackTask();
	~ASyncUnpackTask();

	void setInfoHead(const std::string&);
	void setPackage(const std::string&);
	void setPath(const std::string&);

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();

private:
	std::string package_;
	std::string path_;
	std::string infoHead_;
	std::string infoBody_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCUNPACKTASK_HEADER_

