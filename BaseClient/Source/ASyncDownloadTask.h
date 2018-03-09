#ifndef ASYNCDOWNLOADTASK_HEADER_
#define ASYNCDOWNLOADTASK_HEADER_

#include "ASyncTask.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncDownloadTask : public ASyncTask {
public:
	explicit ASyncDownloadTask(Rpc::DownloaderPrx downloader);
	ASyncDownloadTask(Rpc::DownloaderPrx downloader, bool autoRetry);
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
	bool checkCancelled();

private:
	Rpc::DownloaderPrx downloader_;
	bool autoRetry_;

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

