#ifndef ASYNCDOWNLOADCLIENTTASK_HEADER_
#define ASYNCDOWNLOADCLIENTTASK_HEADER_

#include "ASyncTask.h"

#include <RpcSession.h>

#include <boost/thread/mutex.hpp>

#include <memory>
#include <thread>
#include <string>

class ASyncDownloadClientTask : public ASyncTask {
public:
	ASyncDownloadClientTask(Rpc::DownloaderPrx downloader);
	~ASyncDownloadClientTask();

	void setInfoHead(const std::string&);

	std::string path();

	virtual void start();
	virtual void cancel();

	virtual int state();
	virtual int progress();
	virtual std::string information();

private:
	void run();
	int update(ASyncTask*, int, double);
	std::string uniquePath();

private:
	Rpc::DownloaderPrx downloader_;

	std::string infoHead_;
	std::string path_;
	std::string info_;

	int state_;
	int progress_;
	bool cancelled_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

#endif // ASYNCDOWNLOADCLIENTTASK_HEADER_

