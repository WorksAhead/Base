#include "ASyncInstallEngineTask.h"
#include "ASyncDownloadTask.h"
#include "AsyncUnpackTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncInstallEngineTask::ASyncInstallEngineTask(ContextPtr context, Rpc::DownloaderPrx downloader) : context_(context), downloader_(downloader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncInstallEngineTask::~ASyncInstallEngineTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncInstallEngineTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncInstallEngineTask::setEngineVersion(const EngineVersion& v)
{
	engineVersion_ = v;
}

void ASyncInstallEngineTask::setPath(const std::string& path)
{
	path_ = path;
}

void ASyncInstallEngineTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncInstallEngineTask::run, this)));
}

void ASyncInstallEngineTask::cancel()
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

int ASyncInstallEngineTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncInstallEngineTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncInstallEngineTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncInstallEngineTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	info_ = infoHead_;
	sync_.unlock();

	int state = context_->getEngineState(engineVersion_);
	if (state != EngineState::installing) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "Bad state";
		state_ = ASyncTask::state_failed;
		return;
	}

	const std::string& packageFilename = context_->uniquePath() + ".package";

	BOOST_SCOPE_EXIT_ALL(&packageFilename)
	{
		boost::system::error_code ec;
		if (fs::exists(packageFilename, ec)) {
			fs::remove(packageFilename, ec);
		}
	};

	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(this, &commit)
	{
		int state = EngineState::installing;
		if (!commit) {
			context_->changeEngineState(engineVersion_, state, EngineState::not_installed);
		}
	};

	std::unique_ptr<ASyncDownloadTask> downloadTask(new ASyncDownloadTask(context_, downloader_));
	downloadTask->setInfoHead(infoHead_);
	downloadTask->setFilename(packageFilename);
	downloadTask->start();

	for (;;)
	{
		const int ret = update(downloadTask.get(), 0, 0.5);
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

	std::unique_ptr<ASyncUnpackTask> unpackTask(new ASyncUnpackTask(context_));
	unpackTask->setInfoHead(infoHead_);
	unpackTask->setPackage(packageFilename);
	unpackTask->setPath(path_);
	unpackTask->start();

	for (;;)
	{
		const int ret = update(unpackTask.get(), 50, 0.5);
		if (ret < 0) {
			return;
		}
		else if (ret > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	context_->setupEngine(engineVersion_);

	if (!context_->changeEngineState(engineVersion_, state, EngineState::installed)) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "Bad state";
		state_ = ASyncTask::state_failed;
		return;
	}

	commit = true;

	context_->addEngineToGui(engineVersion_);

	sync_.lock();
	info_ = infoHead_;
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

int ASyncInstallEngineTask::update(ASyncTask* task, int a, double b)
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

