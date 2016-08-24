#include "ASyncRemoveTask.h"
#include "Package.h"
#include "FileScanner.h"
#include "ErrorMessage.h"
#include "Crc.h"
#include "PathUtils.h"

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
#define CHECK_EC(ec)								\
	if (ec) {										\
		boost::mutex::scoped_lock lock(sync_);		\
		infoBody_ = ec.message();					\
		state_ = ASyncTask::state_failed;			\
		return;										\
	}

	sync_.lock();
	state_ = ASyncTask::state_running;
	sync_.unlock();

	sync_.lock();
	infoBody_ = "Removing";
	sync_.unlock();

	boost::system::error_code ec;

	fs::path path = normalizePath(path_);

	std::list<fs::directory_iterator> stack(1, fs::directory_iterator(path, ec));
	CHECK_EC(ec);

	for (;;)
	{
		fs::directory_iterator& it = stack.back();

		if (it != fs::directory_iterator())
		{
			fs::path p = normalizePath(it->path());

			if (boost::filesystem::is_directory(p, ec))
			{
				CHECK_EC(ec);
				stack.push_back(fs::directory_iterator(p, ec));
				CHECK_EC(ec);
			}
			else
			{
				CHECK_EC(ec);
				fs::remove(p, ec);
				CHECK_EC(ec);
				++it;
			}
		}
		else
		{
			stack.pop_back();
			if (stack.empty()) {
				break;
			}

			fs::path p = normalizePath(stack.back()->path());
			fs::remove(p, ec);
			CHECK_EC(ec);

			++stack.back();
		}
	}

	fs::remove(path, ec);
	CHECK_EC(ec);

	sync_.lock();
	infoBody_.clear();
	state_ = ASyncTask::state_finished;
	progress_ = 100;
	sync_.unlock();

#undef CHECK_EC
}

