#include "ASyncRemoveEngineTask.h"
#include "ASyncRemoveTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncRemoveEngineTask::ASyncRemoveEngineTask(ContextPtr context) : context_(context)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncRemoveEngineTask::~ASyncRemoveEngineTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncRemoveEngineTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncRemoveEngineTask::setEngineVersion(const EngineVersion& v)
{
	engineVersion_ = v;
}

void ASyncRemoveEngineTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncRemoveEngineTask::run, this)));
}

void ASyncRemoveEngineTask::cancel()
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

int ASyncRemoveEngineTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncRemoveEngineTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncRemoveEngineTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncRemoveEngineTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	info_ = infoHead_;
	sync_.unlock();

	int state = context_->getEngineState(engineVersion_);
	if (state != EngineState::removing) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "Bad state";
		state_ = ASyncTask::state_failed;
		return;
	}

	std::unique_ptr<ASyncRemoveTask> removeTask(new ASyncRemoveTask);
	removeTask->setInfoHead(infoHead_);
	removeTask->setPath(context_->enginePath(engineVersion_));
	removeTask->start();

	for (;;)
	{
		const int ret = update(removeTask.get(), 0, 1.0);
		if (ret < 0) {
			return;
		}
		else if (ret > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	if (!context_->changeEngineState(engineVersion_, state, EngineState::not_installed)) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "Bad state";
		state_ = ASyncTask::state_failed;
		return;
	}

	sync_.lock();
	info_ = infoHead_;
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

int ASyncRemoveEngineTask::update(ASyncTask* task, int a, double b)
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

