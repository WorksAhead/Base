#ifndef ASYNCREMOVEENGINETASK_HEADER_
#define ASYNCREMOVEENGINETASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncRemoveEngineTask : public ASyncTask {
public:
	ASyncRemoveEngineTask(ContextPtr context);
	~ASyncRemoveEngineTask();

	void setInfoHead(const std::string&);
	void setEngineVersion(const EngineVersion&);

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

	std::string infoHead_;
	EngineVersion engineVersion_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCREMOVEENGINETASK_HEADER_

