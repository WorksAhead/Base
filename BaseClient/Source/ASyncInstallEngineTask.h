#ifndef ASYNCINSTALLENGINETASK_HEADER_
#define ASYNCINSTALLENGINETASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncInstallEngineTask : public ASyncTask {
public:
	ASyncInstallEngineTask(ContextPtr context, Rpc::DownloaderPrx downloader);
	~ASyncInstallEngineTask();

	void setInfoHead(const std::string&);
	void setEngineVersion(const std::string& name, const std::string& version);
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
	std::string engineName_;
	std::string engineVersion_;
	std::string path_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCINSTALLENGINETASK_HEADER_

