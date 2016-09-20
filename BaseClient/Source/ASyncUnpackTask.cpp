#include "ASyncUnpackTask.h"
#include "Package.h"
#include "ErrorMessage.h"
#include "Crc.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <vector>

namespace fs = boost::filesystem;

ASyncUnpackTask::ASyncUnpackTask()
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncUnpackTask::~ASyncUnpackTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncUnpackTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncUnpackTask::setPackage(const std::string& package)
{
	package_ = package;
}

void ASyncUnpackTask::setPath(const std::string& path)
{
	path_ = path;
}

void ASyncUnpackTask::start()
{
	t_.reset(new std::thread([this](){
		try {
			run();
		}
		catch (Ice::Exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = std::string("Rpc: ") + e.what();
			state_ = ASyncTask::state_failed;
		}
		catch (std::exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = e.what();
			state_ = ASyncTask::state_failed;
		}
		catch (...) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "unknown exception";
			state_ = ASyncTask::state_failed;
		}
	}));
}

void ASyncUnpackTask::cancel()
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

int ASyncUnpackTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncUnpackTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncUnpackTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncUnpackTask::run()
{
	const int level = 0;

	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
	}

	std::shared_ptr<Unpacker> unpacker(new Unpacker(package_, path_));

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Unpacking";
	}

	size_t lastIndex = -1;
	Packer::Path lastUnpackingFile;

	for (;;)
	{
		{
			boost::mutex::scoped_lock lock(sync_);
			if (cancelled_) {
				unpacker.reset();
				infoBody_.clear();
				state_ = ASyncTask::state_cancelled;
				return;
			}
		}

		const int ret = unpacker->executeStep();

		if (unpacker->currentIndex() != lastIndex) {
			lastIndex = unpacker->currentIndex();
			boost::mutex::scoped_lock lock(sync_);
			progress_ = (double)lastIndex / (double)unpacker->count() * 100.0;
		}

		if (unpacker->currentFile() != lastUnpackingFile) {
			lastUnpackingFile = unpacker->currentFile();
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Unpacking " + lastUnpackingFile.string();
		}

		if (ret == 0) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_.clear();
			break;
		}
		else if (ret < 0) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = unpacker->errorMessage();
			state_ = ASyncTask::state_failed;
			return;
		}
	}

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
		progress_ = 100;
	}
}

