#include "ASyncSubmitExtraTask.h"
#include "AsyncPackTask.h"
#include "ASyncUploadTask.h"
#include "ErrorMessage.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

ASyncSubmitExtraTask::ASyncSubmitExtraTask(ContextPtr context, Rpc::ExtraSubmitterPrx submitter) : context_(context), submitter_(submitter)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncSubmitExtraTask::~ASyncSubmitExtraTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncSubmitExtraTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncSubmitExtraTask::setExtraLocation(const std::string& path)
{
	extraLocation_ = path;
}

void ASyncSubmitExtraTask::setImageFile(const std::string& imageFile)
{
	imageFile_ = imageFile;
}

void ASyncSubmitExtraTask::start()
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

void ASyncSubmitExtraTask::cancel()
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

int ASyncSubmitExtraTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncSubmitExtraTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncSubmitExtraTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncSubmitExtraTask::run()
{
	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
		info_ = infoHead_;
	}

	if (!imageFile_.empty())
	{
		Rpc::UploaderPrx uploader;
		Rpc::ErrorCode ec = submitter_->uploadImage(uploader);
		if (ec != Rpc::ec_success) {
			boost::mutex::scoped_lock lock(sync_);
			info_ = infoHead_ + " - " + std::string("Rpc: ") + errorMessage(ec);
			state_ = ASyncTask::state_failed;
			return;
		}

		std::unique_ptr<ASyncUploadTask> task(new ASyncUploadTask(context_, uploader));
		task->setInfoHead(infoHead_);
		task->setFilename(imageFile_);
		task->start();

		for (;;)
		{
			const int ret = update(task.get(), 0, 0.01);
			if (ret < 0) {
				if (task->state() == ASyncTask::state_cancelled) {
					uploader->cancel();
				}
				return;
			}
			else if (ret > 0) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	Rpc::UploaderPrx uploader;
	Rpc::ErrorCode ec = submitter_->uploadExtra(uploader);
	if (ec != Rpc::ec_success) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	std::unique_ptr<ASyncPackTask> packTask(new ASyncPackTask(context_));
	packTask->setInfoHead(infoHead_);
	packTask->setPath(extraLocation_);
	packTask->start();

	for (;;)
	{
		const int ret = update(packTask.get(), 1, 0.49);
		if (ret < 0) {
			if (packTask->state() == ASyncTask::state_cancelled) {
				submitter_->cancel();
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

	std::unique_ptr<ASyncUploadTask> uploadTask(new ASyncUploadTask(context_, uploader));
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

	ec = submitter_->finish();
	if (ec != Rpc::ec_success) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	{
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_;
		state_ = ASyncTask::state_finished;
	}
}

int ASyncSubmitExtraTask::update(ASyncTask* task, int a, double b)
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

