#include "ASyncInstallExtraTask.h"
#include "ASyncDownloadTask.h"
#include "AsyncUnpackTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncInstallExtraTask::ASyncInstallExtraTask(ContextPtr context, Rpc::DownloaderPrx downloader) : context_(context), downloader_(downloader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncInstallExtraTask::~ASyncInstallExtraTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncInstallExtraTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncInstallExtraTask::setId(const std::string& id)
{
	id_ = id;
}

void ASyncInstallExtraTask::setPath(const std::string& path)
{
	path_ = path;
}

void ASyncInstallExtraTask::start()
{
	t_.reset(new std::thread([this](){
		try {
			run();
		}
		catch (Ice::Exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			info_ = infoHead_ + " - " + "Rpc: " + e.what();
			state_ = ASyncTask::state_failed;
		}
		catch (std::exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			info_ = infoHead_ + " - " + e.what();
			state_ = ASyncTask::state_failed;
		}
		catch (...) {
			boost::mutex::scoped_lock lock(sync_);
			info_ = infoHead_ + " - " + "unknown exception";
			state_ = ASyncTask::state_failed;
		}
	}));
}

void ASyncInstallExtraTask::cancel()
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

int ASyncInstallExtraTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncInstallExtraTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncInstallExtraTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncInstallExtraTask::run()
{
	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
		info_ = infoHead_;
	}

	int state = context_->getExtraState(id_);
	if (state != ExtraState::downloading) {
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
		int state = ExtraState::downloading;
		if (!commit) {
			context_->changeExtraState(id_, state, ExtraState::not_downloaded);
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

	context_->setupExtra(id_);

	if (!context_->changeExtraState(id_, state, ExtraState::downloaded)) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "Bad state";
		state_ = ASyncTask::state_failed;
		return;
	}

	commit = true;

	context_->addExtraToGui(id_);

	{
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_;
		state_ = ASyncTask::state_finished;
	}
}

int ASyncInstallExtraTask::update(ASyncTask* task, int a, double b)
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

