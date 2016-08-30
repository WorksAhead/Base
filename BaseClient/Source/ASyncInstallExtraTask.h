#ifndef ASYNCINSTALLEXTRATASK_HEADER_
#define ASYNCINSTALLEXTRATASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncInstallExtraTask : public ASyncTask {
public:
	ASyncInstallExtraTask(ContextPtr context, Rpc::DownloaderPrx downloader);
	~ASyncInstallExtraTask();

	void setInfoHead(const std::string&);
	void setId(const std::string&);
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
	Rpc::DownloaderPrx downloader_;

	std::string infoHead_;
	std::string id_;
	std::string path_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCINSTALLEXTRATASK_HEADER_

