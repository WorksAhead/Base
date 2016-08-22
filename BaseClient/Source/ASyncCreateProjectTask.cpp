#include "ASyncCreateProjectTask.h"
#include "AsyncUnpackTask.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncCreateProjectTask::ASyncCreateProjectTask(ContextPtr context) : context_(context)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncCreateProjectTask::~ASyncCreateProjectTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncCreateProjectTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncCreateProjectTask::setContentId(const std::string& id)
{
	contentId_ = id;
}

void ASyncCreateProjectTask::setProjectId(const std::string& id)
{
	projectId_ = id;
}

void ASyncCreateProjectTask::setProperties(const std::map<std::string, std::string>& properties)
{
	properties_ = properties;
}

void ASyncCreateProjectTask::setLocation(const std::string& location)
{
	location_ = location;
}

void ASyncCreateProjectTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncCreateProjectTask::run, this)));
}

void ASyncCreateProjectTask::cancel()
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

int ASyncCreateProjectTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncCreateProjectTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncCreateProjectTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncCreateProjectTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	info_ = infoHead_;
	sync_.unlock();

	const std::string& package = (fs::path(context_->contentPath(contentId_)) / "content").string();

	if (!fs::exists(package)) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + "The content file is missing";
		state_ = ASyncTask::state_failed;
		return;
	}

	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(this, &commit)
	{
		if (!commit) {
			if (fs::exists(location_)) {
				fs::remove_all(location_);
			}
		}
	};

	std::unique_ptr<ASyncUnpackTask> unpackTask(new ASyncUnpackTask(context_));
	unpackTask->setInfoHead(infoHead_);
	unpackTask->setPackage(package);
	unpackTask->setPath(location_);
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

	context_->addProject(projectId_, contentId_, location_, properties_);
	context_->addProjectToGui(projectId_);

	commit = true;

	sync_.lock();
	info_ = infoHead_;
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

int ASyncCreateProjectTask::update(ASyncTask* task, int a, double b)
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

