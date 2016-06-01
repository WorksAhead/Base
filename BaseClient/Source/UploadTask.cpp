#include "UploadTask.h"
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

UploadTask::UploadTask(ContextPtr context, std::string infoHead, const std::string& path, Rpc::UploaderPrx uploader)
	: context_(context), infoHead_(infoHead), path_(path), uploader_(uploader)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancel_ = false;
}

UploadTask::~UploadTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void UploadTask::start()
{
	t_.reset(new std::thread(std::bind(&UploadTask::run, this)));
}

void UploadTask::cancel()
{
	boost::unique_lock<boost::mutex> lock(sync_);
	if (cancel_ || state_ != ASyncTask::state_running) {
		return;
	}
	state_ = ASyncTask::state_cancelling;
	cancel_ = true;
	lock.unlock();
	t_->join();
}

int UploadTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int UploadTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string UploadTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void UploadTask::run()
{
	const int level = 0;

	bool commit = false;

	BOOST_SCOPE_EXIT_ALL(&commit, this)
	{
		if (!commit) {
			uploader_->cancel();
		}
	};

	sync_.lock();
	state_ = ASyncTask::state_running;
	sync_.unlock();

	std::vector<FileScanner::Path> srcFiles;

	FileScanner scanner(path_);

	sync_.lock();
	infoBody_ = "Scanning";
	sync_.unlock();

	for (;;)
	{
		sync_.lock();
		if (cancel_) {
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

	const std::string& packFile = context_->uniquePath() + ".package";

	BOOST_SCOPE_EXIT_ALL(&packFile)
	{
		boost::system::error_code ec;
		if (fs::exists(packFile, ec)) {
			fs::remove(packFile, ec);
		}
	};

	std::shared_ptr<Packer> packer(new Packer(packFile, path_, srcFiles, level));

	sync_.lock();
	infoBody_ = "Packing";
	sync_.unlock();

	Packer::Path lastPackingFile;

	for (;;)
	{
		sync_.lock();
		if (cancel_) {
			packer.reset();
			boost::system::error_code ec;
			fs::remove(packFile, ec);
			infoBody_.clear();
			state_ = ASyncTask::state_cancelled;
			sync_.unlock();
			return;
		}
		sync_.unlock();

		const int ret = packer->executeStep();

		if (packer->currentFile() != lastPackingFile) {
			lastPackingFile = packer->currentFile();
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Packing " + lastPackingFile.string();
		}

		if (ret == 0) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_.clear();
			break;
		}
		else if (ret < 0) {
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = packer->errorMessage();
			state_ = ASyncTask::state_failed;
			return;
		}
	}

	std::fstream is(packFile.c_str(), std::ios::in|std::ios::binary);
	if (!is.is_open()) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Failed to open package file \"" + packFile + "\"";
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

		sync_.lock();
		infoBody_ = "Transferring";
		sync_.unlock();

		BOOST_SCOPE_EXIT_ALL(&buffers)
		{
			for (Buffer& buf : buffers) {
				if (buf.second) {
					buf.second->cancel();
					buf.second = 0;
				}
			}
		};

		while (remain)
		{
			sync_.lock();
			if (cancel_) {
				infoBody_.clear();
				state_ = ASyncTask::state_cancelled;
				sync_.unlock();
				return;
			}
			progress_ = (int)(double(offset) / (double)(offset + remain) * 100.0);
			sync_.unlock();

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
				infoBody_ = "Failed to read package file \"" + packFile + "\"";
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

	sync_.lock();
	if (cancel_) {
		infoBody_.clear();
		state_ = ASyncTask::state_cancelled;
		sync_.unlock();
		return;
	}
	progress_ = 100;
	infoBody_ = "Verifying";
	sync_.unlock();

	Rpc::ErrorCode ec = uploader_->finish(crc.value());
	if (ec != Rpc::ec_success) {
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = std::string("Rpc: ") + errorMessage(ec);
		state_ = ASyncTask::state_failed;
		return;
	}

	commit = true;

	sync_.lock();
	infoBody_.clear();
	state_ = ASyncTask::state_finished;
	sync_.unlock();
}

