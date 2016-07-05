#include "ASyncDownloadTask.h"
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

ASyncDownloadTask::ASyncDownloadTask(ContextPtr context, Rpc::DownloaderPrx downloader) : context_(context), downloader_(downloader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncDownloadTask::~ASyncDownloadTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncDownloadTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncDownloadTask::setFilename(const std::string& filename)
{
	filename_ = filename;
}

std::string ASyncDownloadTask::filename()
{
	boost::mutex::scoped_lock lock(sync_);
	return filename_;
}

void ASyncDownloadTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncDownloadTask::run, this)));
}

void ASyncDownloadTask::cancel()
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

int ASyncDownloadTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncDownloadTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncDownloadTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncDownloadTask::run()
{
	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(&commit, this)
	{
		downloader_->finish();
		if (!commit) {
			boost::system::error_code ec;
			if (fs::exists(filename_, ec)) {
				fs::remove(filename_, ec);
			}
		}
	};

	sync_.lock();
	state_ = ASyncTask::state_running;
	sync_.unlock();

	std::fstream os(filename_.c_str(), std::ios::out|std::ios::binary);
	if (!os.is_open()) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Failed to open file \"" + filename_ + "\"";
		state_ = ASyncTask::state_failed;
		return;
	}

	{
		Ice::Long length = 0;

		Rpc::ErrorCode ec = downloader_->getSize(length);
		if (ec != Rpc::ec_success) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = std::string("Rpc: ") + errorMessage(ec);
			state_ = ASyncTask::state_failed;
			return;
		}

		std::vector<Ice::AsyncResultPtr> asyncResults(2);

		BOOST_SCOPE_EXIT_ALL(&asyncResults)
		{
			for (Ice::AsyncResultPtr& result : asyncResults) {
				if (result) {
					result->cancel();
					result = 0;
				}
			}
		};

		sync_.lock();
		infoBody_ = "Transferring";
		sync_.unlock();

		int current = 0;
		Ice::Long remain = length;
		Ice::Long offset = 0;
		std::streamsize wrote = 0;

		while (wrote < length)
		{
			sync_.lock();
			if (cancelled_) {
				infoBody_.clear();
				state_ = ASyncTask::state_cancelled;
				sync_.unlock();
				return;
			}
			progress_ = (int)(double(offset) / (double)(offset + remain) * 100.0);
			sync_.unlock();

			Ice::AsyncResultPtr& result = asyncResults[current];

			if (result)
			{
				Rpc::ByteSeq buf;
				ec = downloader_->end_read(buf, result);
				result = 0;
				if (ec != Rpc::ec_success) {
					boost::mutex::scoped_lock lock(sync_);
					infoBody_ = std::string("Rpc: ") + errorMessage(ec);
					state_ = ASyncTask::state_failed;
					return;
				}
				if (!os.write((const char*)&buf[0], buf.size())) {
					boost::mutex::scoped_lock lock(sync_);
					infoBody_ = "Failed to write file \"" + filename_ + "\"";
					state_ = ASyncTask::state_failed;
					return;
				}
				wrote += buf.size();
			}

			const Ice::Int n = (Ice::Int)std::min<Ice::Long>(remain, 1024*128);
			if (n > 0) {
				result = downloader_->begin_read(offset, n);
			}

			if (++current == asyncResults.size()) {
				current = 0;
			}

			remain -= n;
			offset += n;
		}
	}

	sync_.lock();
	if (cancelled_) {
		infoBody_.clear();
		state_ = ASyncTask::state_cancelled;
		sync_.unlock();
		return;
	}
	sync_.unlock();

	commit = true;

	sync_.lock();
	infoBody_.clear();
	state_ = ASyncTask::state_finished;
	progress_ = 100;
	sync_.unlock();
}

