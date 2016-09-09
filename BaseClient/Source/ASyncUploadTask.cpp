#include "ASyncUploadTask.h"
#include "Package.h"
#include "ErrorMessage.h"
#include "Crc.h"
#include "PathUtils.h"

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

ASyncUploadTask::ASyncUploadTask(ContextPtr context, Rpc::UploaderPrx uploader) : context_(context), uploader_(uploader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncUploadTask::~ASyncUploadTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncUploadTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncUploadTask::setFilename(const std::string& filename)
{
	filename_ = filename;
}

void ASyncUploadTask::start()
{
	t_.reset(new std::thread([this](){
		try {
			run();
		}
		catch(Ice::Exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = std::string("Rpc: ") + e.what();
			state_ = ASyncTask::state_failed;
		}
		catch(std::exception& e) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = e.what();
			state_ = ASyncTask::state_failed;
		}
		catch(...) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "unknown exception";
			state_ = ASyncTask::state_failed;
		}
	}));
}

void ASyncUploadTask::cancel()
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

int ASyncUploadTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncUploadTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncUploadTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncUploadTask::run()
{
	const int level = 0;

	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(&commit, this)
	{
		if (!commit) {
			uploader_->cancel();
		}
	};

	std::string safeFilename = makeSafePath(filename_);

	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
	}

	std::fstream is(safeFilename.c_str(), std::ios::in|std::ios::binary);
	if (!is.is_open()) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Failed to open file \"" + filename_ + "\"";
		state_ = ASyncTask::state_failed;
		return;
	}

	Crc32 crc;

	{
		is.seekg(0, std::ios::end);
		std::streamsize remain = is.tellg();
		is.seekg(0, std::ios::beg);

		typedef std::pair<std::vector<char>, Ice::AsyncResultPtr> Buffer;

		std::vector<Buffer> buffers(2);

		for (Buffer& buf : buffers) {
			buf.first.resize(1024 * 128);
		}

		size_t current = 0;

		std::streamoff offset = 0;
		std::streamoff lastOffset = 0;

		{
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Transferring";
		}

		BOOST_SCOPE_EXIT_ALL(&buffers)
		{
			for (Buffer& buf : buffers) {
				if (buf.second) {
					buf.second->cancel();
					buf.second = 0;
				}
			}
		};

		boost::chrono::steady_clock::time_point lastTimePoint = boost::chrono::steady_clock::now();

		while (remain)
		{
			{
				boost::mutex::scoped_lock lock(sync_);
				if (cancelled_) {
					infoBody_.clear();
					state_ = ASyncTask::state_cancelled;
					return;
				}
				progress_ = (int)(double(offset) / (double)(offset + remain) * 100.0);
			}

			{
				boost::chrono::steady_clock::time_point t = boost::chrono::steady_clock::now();
				boost::chrono::milliseconds d = boost::chrono::duration_cast<boost::chrono::milliseconds>(t - lastTimePoint);
				if (d.count() >= 1000)
				{
					double s = (offset - lastOffset) / (d.count() * 0.001);
					if (s >= 1024.0*1024.0) {
						s = s / (1024.0*1024.0);
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fMBytes/s") % s).str();
					}
					else if (s >= 1024.0) {
						s = s / 1024.0;
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fKBytes/s") % s).str();
					}
					else {
						boost::mutex::scoped_lock lock(sync_);
						infoBody_ = (boost::format("Transferring %.2fBytes/s") % s).str();
					}
					lastOffset = offset;
					lastTimePoint = t;
				}
			}

			Buffer& buf = buffers[current];

			if (buf.second) {
				Rpc::ErrorCode ec = uploader_->end_write(buf.second);
				buf.second = 0;
				if (ec != Rpc::ec_success) {
					boost::mutex::scoped_lock lock(sync_);
					infoBody_ = std::string("Rpc: ") + errorMessage(ec);
					state_ = ASyncTask::state_failed;
					return;
				}
			}

			const std::streamsize n = std::min<std::streamsize>(remain, buf.first.size());

			if (!is.read(&buf.first[0], n)) {
				boost::mutex::scoped_lock lock(sync_);
				infoBody_ = "Failed to read file \"" + filename_ + "\"";
				state_ = ASyncTask::state_failed;
				return;
			}

			crc.update(&buf.first[0], n);

			buf.second = uploader_->begin_write(offset, std::make_pair((const Ice::Byte*)&buf.first[0], ((const Ice::Byte*)&buf.first[0]) + n));

			if (++current == buffers.size()) {
				current = 0;
			}

			remain -= n;
			offset += n;
		}

		for (Buffer& buf : buffers) {
			if (buf.second) {
				Rpc::ErrorCode ec = uploader_->end_write(buf.second);
				buf.second = 0;
				if (ec != Rpc::ec_success) {
					boost::mutex::scoped_lock lock(sync_);
					infoBody_ = std::string("Rpc: ") + errorMessage(ec);
					state_ = ASyncTask::state_failed;
					return;
				}
			}
		}
	}

	{
		boost::mutex::scoped_lock lock(sync_);
		if (cancelled_) {
			infoBody_.clear();
			state_ = ASyncTask::state_cancelled;
			return;
		}
		progress_ = 100;
		infoBody_ = "Verifying";
	}

	Rpc::ErrorCode ec = uploader_->finish(crc.value());
	if (ec != Rpc::ec_success) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	commit = true;

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
	}
}

