#ifndef ASYNCDOWNLOADCONTENTTASK_HEADER_
#define ASYNCDOWNLOADCONTENTTASK_HEADER_

#include "ASyncTask.h"
#include "ASyncDownloadTask.h"
#include "AsyncUnpackTask.h"
#include "Context.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncDownloadContentTask : public ASyncTask {
public:
	ASyncDownloadContentTask(ContextPtr context, Rpc::DownloaderPrx downloader);
	~ASyncDownloadContentTask();

	void setInfoHead(const std::string&);
	void setFilename(const std::string&);
	void setContentId(const std::string& id);

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
	std::string filename_;
	std::string contentId_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCDOWNLOADCONTENTTASK_HEADER_

