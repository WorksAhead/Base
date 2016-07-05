#ifndef ASYNCDOWNLOADTASK_HEADER_
#define ASYNCDOWNLOADTASK_HEADER_

#include "ASyncTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncDownloadTask : public ASyncTask {
public:
	ASyncDownloadTask(ContextPtr context, Rpc::DownloaderPrx downloader);
	~ASyncDownloadTask();

	void setInfoHead(const std::string&);
	void setFilename(const std::string&);

	std::string filename();

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();

private:
	ContextPtr context_;
	Rpc::DownloaderPrx downloader_;

	std::string filename_;
	std::string infoHead_;
	std::string infoBody_;

	int state_;
	int progress_;
	bool cancelled_;

	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCDOWNLOADTASK_HEADER_

