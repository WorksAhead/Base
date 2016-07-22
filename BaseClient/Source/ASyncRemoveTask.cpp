#include "ASyncRemoveTask.h"
#include "Package.h"
#include "FileScanner.h"
#include "ErrorMessage.h"
#include "Crc.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory.h>

namespace fs = boost::filesystem;

ASyncRemoveTask::ASyncRemoveTask()
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncRemoveTask::~ASyncRemoveTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncRemoveTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncRemoveTask::setPath(const std::string& path)
{
	path_ = path;
}

void ASyncRemoveTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncRemoveTask::run, this)));
}

void ASyncRemoveTask::cancel()
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

int ASyncRemoveTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncRemoveTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncRemoveTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncRemoveTask::run()
{
	sync_.lock();
	state_ = ASyncTask::state_running;
	sync_.unlock();

	if (!fs::exists(path_)) {
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_finished;
		progress_ = 100;
		return;
	}

	std::vector<FileScanner::Path> srcFiles;

	FileScanner scanner(path_);

	sync_.lock();
	infoBody_ = "Scanning";
	sync_.unlock();

	for (;;)
	{
		sync_.lock();
		if (cancelled_) {
			infoBody_.clear();
			state_ = ASyncTask::state_cancelled;
			sync_.unlock();
			return;
		}
		sync_.unlock();

		FileScanner::Path path;
		int ret = scanner.nextFile(path);
		if (ret > 0) {
			srcFiles.push_back(path);
		}
		else if (ret < 0) {
			break;
		}
	}

	sync_.lock();
	infoBody_ = "Removing";
	sync_.unlock();

	for (size_t i = 0; i < srcFiles.size(); ++i)
	{
		sync_.lock();
		if (cancelled_) {
			infoBody_.clear();
			state_ = ASyncTask::state_cancelled;
			sync_.unlock();
			return;
		}
		sync_.unlock();

		FileScanner::Path p = path_ / srcFiles[i] ;

		sync_.lock();
		infoBody_ = "Removing " + p.generic_string();
		sync_.unlock();

		if (!fs::remove(p)) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Unable to remove " + p.generic_string();
			state_ = ASyncTask::state_failed;
			return;
		}
		else {
			boost::mutex::scoped_lock lock(sync_);
			progress_ = (double)i / (double)srcFiles.size() * 100.0;
		}
	}

	sync_.lock();
	infoBody_ = "Removing " + path_;
	sync_.unlock();

	fs::remove_all(path_);

	sync_.lock();
	infoBody_.clear();
	state_ = ASyncTask::state_finished;
	progress_ = 100;
	sync_.unlock();
}

