#include "ASyncSubmitClientTask.h"
#include "AsyncPackTask.h"
#include "ASyncUploadTask.h"
#include "PathUtils.h"
#include "ErrorMessage.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <sstream>

namespace fs = boost::filesystem;

ASyncSubmitClientTask::ASyncSubmitClientTask(ContextPtr context, Rpc::ClientSubmitterPrx submitter) : context_(context), submitter_(submitter)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncSubmitClientTask::~ASyncSubmitClientTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncSubmitClientTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncSubmitClientTask::setPath(const std::string& path)
{
	path_ = path;
}

void ASyncSubmitClientTask::start()
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

void ASyncSubmitClientTask::cancel()
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

int ASyncSubmitClientTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncSubmitClientTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncSubmitClientTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	return info_;
}

void ASyncSubmitClientTask::run()
{
	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
		info_ = infoHead_;
	}

	Rpc::UploaderPrx uploader;
	Rpc::ErrorCode ec = submitter_->uploadClient(uploader);
	if (ec != Rpc::ec_success) {
		boost::mutex::scoped_lock lock(sync_);
		info_ = infoHead_ + " - " + std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	std::string srcFiles;

	{
		fs::path safePath = makeSafePath(path_);

		std::fstream is((safePath / "update").string().c_str(), std::ios::in);
		if (!is.is_open()) {
			boost::mutex::scoped_lock lock(sync_);
			info_ = infoHead_ + " - " + "Update file not exists";
			state_ = ASyncTask::state_failed;
			return;
		}

		std::ostringstream oss;

		std::string line;
		while (std::getline(is, line)) {
			if (boost::starts_with(line, "cp ")) {
				oss << std::string(line.c_str() + 3) << "\n";
			}
		}

		oss << "update\n";

		srcFiles = oss.str();
	}

	std::unique_ptr<ASyncPackTask> packTask(new ASyncPackTask(context_));
	packTask->setInfoHead(infoHead_);
	packTask->setPath(path_);
	packTask->setSourceFiles(srcFiles);
	packTask->start();

	for (;;)
	{
		const int ret = update(packTask.get(), 0, 0.5);
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

int ASyncSubmitClientTask::update(ASyncTask* task, int a, double b)
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

