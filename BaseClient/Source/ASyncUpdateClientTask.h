#ifndef ASYNCUPDATECLIENTTASK_HEADER_
#define ASYNCUPDATECLIENTTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncUpdateClientTask : public ASyncTask {
public:
	ASyncUpdateClientTask();
	~ASyncUpdateClientTask();

	void setInfoHead(const std::string&);
	void setSourcePath(const std::string&);
	void setTargetPath(const std::string&);

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();

private:
	std::string infoHead_;
	std::string infoBody_;
	std::string sourcePath_;
	std::string targetPath_;

	int state_;
	int progress_;
	bool cancelled_;

	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCUPDATECLIENTTASK_HEADER_

