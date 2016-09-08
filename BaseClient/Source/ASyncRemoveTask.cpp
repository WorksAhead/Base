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

	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
	}

	boost::system::error_code ec;

	fs::path safePath = makeSafePath(path_);
	fs::path base;

	std::list<fs::directory_iterator> stack(1, fs::directory_iterator(safePath, ec));
	CHECK_EC(ec);

	for (;;)
	{
		{
			boost::mutex::scoped_lock lock(sync_);
			if (cancelled_) {
				infoBody_.clear();
				state_ = ASyncTask::state_cancelled;
				return;
			}
		}

		fs::directory_iterator& it = stack.back();

		if (it != fs::directory_iterator())
		{
			fs::path safeCurrentPath = makeSafePath(it->path());

			if (boost::filesystem::is_directory(safeCurrentPath, ec))
			{
				CHECK_EC(ec);
				stack.push_back(fs::directory_iterator(safeCurrentPath, ec));
				CHECK_EC(ec);
				base = base / safeCurrentPath.leaf();
			}
			else
			{
				CHECK_EC(ec);
				{
					boost::mutex::scoped_lock lock(sync_);
					infoBody_ = "Removing " + (base / safeCurrentPath.leaf()).string();
				}
				fs::remove(safeCurrentPath, ec);
				CHECK_EC(ec);
				++it;
			}
		}
		else
		{
			stack.pop_back();
			base = base.parent_path();

			if (stack.empty()) {
				break;
			}

			fs::path safeCurrentPath = makeSafePath(stack.back()->path());
			{
				boost::mutex::scoped_lock lock(sync_);
				infoBody_ = "Removing " + (base / safeCurrentPath.leaf()).string();
			}
			fs::remove(safeCurrentPath, ec);
			CHECK_EC(ec);

			++stack.back();
		}
	}

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Removing " + safePath.leaf().string();
	}
	fs::remove(safePath, ec);
	CHECK_EC(ec);

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
		progress_ = 100;
	}

#undef CHECK_EC
}

