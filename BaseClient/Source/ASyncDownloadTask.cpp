#include "ASyncDownloadTask.h"
#include "Package.h"
#include "ErrorMessage.h"
#include "Crc.h"
#include "PathUtils.h"

#include <Ice/LocalException.h>

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <boost/chrono.hpp>
#include <boost/format.hpp>

#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory.h>

namespace fs = boost::filesystem;

ASyncDownloadTask::ASyncDownloadTask(Rpc::DownloaderPrx downloader)
	: downloader_(downloader), autoRetry_(false)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncDownloadTask::ASyncDownloadTask(Rpc::DownloaderPrx downloader, bool autoRetry)
	: downloader_(downloader), autoRetry_(autoRetry)
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

	std::string safeFilename = makeSafePath(filename_);

	BOOST_SCOPE_EXIT_ALL(&commit, safeFilename, this)
	{
		if (commit)
		{
			downloader_->finish();
		}
		else
		{
			boost::system::error_code ec;

			if (fs::exists(safeFilename, ec)) {
				fs::remove(safeFilename, ec);
			}
		}
	};

	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
	}

	const fs::path& parentPath = fs::path(filename_).parent_path();
	const fs::path& safeParentPath = fs::path(safeFilename).parent_path();

	if (!fs::exists(safeParentPath))
	{
		boost::system::error_code ec;

		if (!fs::create_directories(safeParentPath, ec))
		{
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Failed to create directory \"" + parentPath.string() + "\"";
			state_ = ASyncTask::state_failed;
			return;
		}
	}

	std::fstream os(safeFilename.c_str(), std::ios::out|std::ios::binary);

	if (!os.is_open())
	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Failed to open file \"" + filename_ + "\"";
		state_ = ASyncTask::state_failed;
		return;
	}

	Ice::Long length = 0;

	Rpc::ErrorCode ec = downloader_->getSize(length);

	if (ec != Rpc::ec_success)
	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	std::vector<Ice::AsyncResultPtr> asyncResults(2);

	BOOST_SCOPE_EXIT_ALL(&asyncResults)
	{
		for (Ice::AsyncResultPtr& result : asyncResults)
		{
			if (result) {
				result->cancel();
				result = 0;
			}
		}
	};

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Transferring";
	}

	bool retry = false;
	int current = 0;
	Ice::Long remain = length;
	Ice::Long offset = 0;
	Ice::Long lastOffset = 0;
	std::streamsize wrote = 0;

	boost::chrono::steady_clock::time_point lastTimePoint = boost::chrono::steady_clock::now();

	for (;;)
	{
		if (retry)
		{
			retry = false;

			{
				boost::mutex::scoped_lock lock(sync_);
				infoBody_ = "Waiting for retry";
			}

			for (int i = 0; i < 50; ++i)
			{
				if (checkCancelled()) {
					return;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		try
		{
			while (wrote < length)
			{
				if (checkCancelled()) {
					return;
				}

				progress_ = (int)(double(offset) / (double)(offset + remain) * 100.0);

				boost::chrono::steady_clock::time_point now = boost::chrono::steady_clock::now();
				boost::chrono::milliseconds elapsed = boost::chrono::duration_cast<boost::chrono::milliseconds>(now - lastTimePoint);

				if (elapsed.count() >= 1000)
				{
					double s = (offset - lastOffset) / (elapsed.count() * 0.001);

					if (s >= 1024.0*1024.0)
					{
						s = s / (1024.0*1024.0);
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fMBytes/s") % s).str();
					}
					else if (s >= 1024.0)
					{
						s = s / 1024.0;
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fKBytes/s") % s).str();
					}
					else
					{
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fBytes/s") % s).str();
					}

					lastOffset = offset;
					lastTimePoint = now;
				}

				Ice::AsyncResultPtr& currentResult = asyncResults[current];

				if (currentResult)
				{
					Rpc::ByteSeq buf;
					ec = downloader_->end_read(buf, currentResult);
					currentResult = 0;

					if (ec != Rpc::ec_success)
					{
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = std::string("Rpc: ") + errorMessage(ec);
						state_ = ASyncTask::state_failed;
						return;
					}

					if (!os.write((const char*)&buf[0], buf.size()))
					{
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = "Failed to write file \"" + filename_ + "\"";
						state_ = ASyncTask::state_failed;
						return;
					}

					wrote += buf.size();
				}

				const Ice::Int n = (Ice::Int)std::min<Ice::Long>(remain, 1024 * 128);

				if (n > 0) {
					currentResult = downloader_->begin_read(offset, n);
				}

				if (++current == asyncResults.size()) {
					current = 0;
				}

				remain -= n;
				offset += n;
			}

			break;
		}
		catch (Ice::SocketException& e)
		{
			if (autoRetry_)
			{
				for (Ice::AsyncResultPtr& result : asyncResults) {
					result = 0;
				}

				current = 0;
				lastOffset = wrote;
				offset = wrote;
				remain = length - wrote;
				retry = true;
			}
			else
			{
				boost::mutex::scoped_lock lock(sync_);
				infoBody_ = "Network error";
				state_ = ASyncTask::state_failed;
				return;
			}
		}
	}

	if (checkCancelled()) {
		return;
	}

	commit = true;

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
		progress_ = 100;
	}
}

bool ASyncDownloadTask::checkCancelled()
{
	boost::mutex::scoped_lock lock(sync_);

	if (cancelled_)
	{
		infoBody_.clear();
		state_ = ASyncTask::state_cancelled;
		return true;
	}

	return false;
}

