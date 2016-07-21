#include "ASyncDownloadContentTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncDownloadContentTask::ASyncDownloadContentTask(ContextPtr context, Rpc::DownloaderPrx downloader) : context_(context), downloader_(downloader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncDownloadContentTask::~ASyncDownloadContentTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncDownloadContentTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncDownloadContentTask::setFilename(const std::string& filename)
{
	filename_ = filename;
}

void ASyncDownloadContentTask::setContentId(const std::string& id)
{
	contentId_ = id;
}

void ASyncDownloadContentTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncDownloadContentTask::run, this)));
}

void ASyncDownloadContentTask::cancel()
{
	boost::unique_lock<boost::mutex> lock(sync_);
	if (cancelled_ || state_ != ASyncTask::state_running) {
		return;
	}
	state_ = ASyncTask::state_cancelling;
	cancelled_ = true;
	lock.unlock();
	t_->join();
}

int ASyncDownloadContentTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncDownloadContentTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncDownloadContentTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncDownloadContentTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	info_ = infoHead_;
	sync_.unlock();

	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(this, &commit)
	{
		int state = ContentState::downloading;
		if (commit) {
			context_->changeContentState(contentId_, state, ContentState::downloaded);
		}
		else {
			context_->changeContentState(contentId_, state, ContentState::not_downloaded);
		}
	};

	std::unique_ptr<ASyncDownloadTask> downloadTask(new ASyncDownloadTask(context_, downloader_));
	downloadTask->setInfoHead(infoHead_);
	downloadTask->setFilename(filename_);
	downloadTask->start();

	for (;;)
	{
		const int ret = update(downloadTask.get(), 0, 1.0);
		if (ret < 0) {
			if (downloadTask->state() == ASyncTask::state_cancelled) {
				downloader_->cancel();
			}
			return;
		}
		else if (ret > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	commit = true;

	sync_.lock();
	info_ = infoHead_;
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

int ASyncDownloadContentTask::update(ASyncTask* task, int a, double b)
{
	boost::mutex::scoped_lock lock(sync_);

	if (cancelled_) {
		task->cancel();
		info_ = infoHead_;
		state_ = ASyncTask::state_cancelled;
		return -1;
	}

	const int state = task->state();

	if (state == ASyncTask::state_running) {
		progress_ = a + task->progress() * b;
		info_ = task->information();
	}
	else if (state == ASyncTask::state_failed) {
		progress_ = a + task->progress() * b;
		info_ = task->information();
		state_ = ASyncTask::state_failed;
		return -2;
	}
	else if (state == ASyncTask::state_finished) {
		progress_ = a + 100 * b;
		info_ = infoHead_;
		return 1;
	}

	return 0;
}

