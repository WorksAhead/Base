#include "ASyncUpdateClientTask.h"
#include "Package.h"
#include "FileScanner.h"
#include "ErrorMessage.h"
#include "Crc.h"
#include "PathUtils.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory.h>

namespace fs = boost::filesystem;

ASyncUpdateClientTask::ASyncUpdateClientTask()
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncUpdateClientTask::~ASyncUpdateClientTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncUpdateClientTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncUpdateClientTask::setSourcePath(const std::string& path)
{
	sourcePath_ = path;
}

void ASyncUpdateClientTask::setTargetPath(const std::string& path)
{
	targetPath_ = path;
}

void ASyncUpdateClientTask::start()
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

void ASyncUpdateClientTask::cancel()
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

int ASyncUpdateClientTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncUpdateClientTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncUpdateClientTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncUpdateClientTask::run()
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

	fs::path safeSrcPath = makeSafePath(sourcePath_);
	fs::path safeTgtPath = makeSafePath(targetPath_);

	std::fstream is((safeSrcPath / "update").string().c_str(), std::ios::in);

	if (!is.is_open()) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Failed to open update file";
		state_ = ASyncTask::state_failed;
		return;
	}

	std::vector<std::string> copyList;
	std::vector<std::string> removeList;

	{
		std::string line;
		while (std::getline(is, line))
		{
			if (boost::starts_with(line, "cp ")) {
				copyList.push_back(line.c_str() + 3);
			}
			else if (boost::starts_with(line, "rm ")) {
				removeList.push_back(line.c_str() + 3);
			}
		}
	}

	const int count = copyList.size() + removeList.size();
	int current = 0;

	if (count == 0)
	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Empty update file";
		state_ = ASyncTask::state_failed;
		return;
	}

	for (int i = 0; i < copyList.size(); ++i)
	{
		fs::path p = copyList[i];

		{
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Copying " + p.string();
		}

		fs::copy_file(safeSrcPath / p, safeTgtPath / p, fs::copy_option::overwrite_if_exists, ec);
		CHECK_EC(ec);

		++current;

		{
			boost::mutex::scoped_lock lock(sync_);
			progress_ = (double)current / (double)count * 100.0;
		}
	}

	for (int i = 0; i < removeList.size(); ++i)
	{
		fs::path p = removeList[i];

		{
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Removing " + p.string();
		}

		if (fs::exists(safeTgtPath / p, ec)) {
			CHECK_EC(ec);
			fs::remove(safeTgtPath / p, ec);
			CHECK_EC(ec);
		}
		CHECK_EC(ec);

		++current;

		{
			boost::mutex::scoped_lock lock(sync_);
			progress_ = (double)current / (double)count * 100.0;
		}
	}

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
		progress_ = 100;
	}

#undef CHECK_EC
}

