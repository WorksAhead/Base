#include "ASyncSubmitEngineTask.h"
#include "AsyncPackTask.h"
#include "ASyncUploadTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncSubmitEngineTask::ASyncSubmitEngineTask(ContextPtr context, Rpc::UploaderPrx uploader) : context_(context), uploader_(uploader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncSubmitEngineTask::~ASyncSubmitEngineTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncSubmitEngineTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncSubmitEngineTask::setPath(const std::string& path)
{
	path_ = path;
}


void ASyncSubmitEngineTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncSubmitEngineTask::run, this)));
}

void ASyncSubmitEngineTask::cancel()
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

int ASyncSubmitEngineTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncSubmitEngineTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncSubmitEngineTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncSubmitEngineTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	info_ = infoHead_;
	sync_.unlock();

	std::unique_ptr<ASyncPackTask> packTask(new ASyncPackTask(context_));
	packTask->setInfoHead(infoHead_);
	packTask->setPath(path_);
	packTask->start();

	for (;;)
	{
		const int ret = update(packTask.get(), 0, 0.5);
		if (ret < 0) {
			if (packTask->state() == ASyncTask::state_cancelled) {
				uploader_->cancel();
			}
			return;
		}
		else if (ret > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	BOOST_SCOPE_EXIT_ALL(&packTask)
	{
		boost::system::error_code ec;
		if (fs::exists(packTask->package(), ec)) {
			fs::remove(packTask->package(), ec);
		}
	};

	std::unique_ptr<ASyncUploadTask> uploadTask(new ASyncUploadTask(context_, uploader_));
	uploadTask->setInfoHead(infoHead_);
	uploadTask->setFilename(packTask->package());
	uploadTask->start();

	for (;;)
	{
		const int ret = update(uploadTask.get(), 50, 0.5);
		if (ret < 0) {
			return;
		}
		else if (ret > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	sync_.lock();
	info_ = infoHead_;
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

int ASyncSubmitEngineTask::update(ASyncTask* task, int a, double b)
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

