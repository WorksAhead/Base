#ifndef ASYNCCREATEPROJECTTASK_HEADER_
#define ASYNCCREATEPROJECTTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncCreateProjectTask : public ASyncTask {
public:
	explicit ASyncCreateProjectTask(ContextPtr context);
	~ASyncCreateProjectTask();

	void setInfoHead(const std::string&);
	void setContentId(const std::string& id);
	void setProjectId(const std::string& id);
	void setProjectName(const std::string& name);
	void setLocation(const std::string& location);

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
	std::string contentId_;
	std::string projectId_;
	std::string projectName_;
	std::string location_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCCREATEPROJECTTASK_HEADER_

