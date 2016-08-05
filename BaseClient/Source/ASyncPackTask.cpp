#include "AsyncPackTask.h"
#include "Package.h"
#include "FileScanner.h"
#include "ErrorMessage.h"
#include "Crc.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <vector>

namespace fs = boost::filesystem;

ASyncPackTask::ASyncPackTask(ContextPtr context) : context_(context)
{
	state_ = ASyncTask::state_idle;
	progress_ = 0;
	cancelled_ = false;
}

ASyncPackTask::~ASyncPackTask()
{
	if (t_.get() && t_->joinable()) {
		t_->join();
	}
}

void ASyncPackTask::setInfoHead(const std::string& infoHead)
{
	infoHead_ = infoHead;
}

void ASyncPackTask::setPath(const std::string& path)
{
	path_ = path;
}

std::string ASyncPackTask::package()
{
	return package_;
}

void ASyncPackTask::start()
{
	t_.reset(new std::thread(std::bind(&ASyncPackTask::run, this)));
}

void ASyncPackTask::cancel()
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

int ASyncPackTask::state()
{
	boost::mutex::scoped_lock lock(sync_);
	return state_;
}

int ASyncPackTask::progress()
{
	boost::mutex::scoped_lock lock(sync_);
	return progress_;
}

std::string ASyncPackTask::information()
{
	boost::mutex::scoped_lock lock(sync_);
	std::string info(infoHead_);
	if (!infoBody_.empty()) {
		info += " - ";
		info += infoBody_;
	}
	return info;
}

void ASyncPackTask::run()
{
#define CHECK_EC(ec)								\
	if (ec) {										\
		boost::mutex::scoped_lock lock(sync_);		\
		infoBody_ = ec.message();					\
		state_ = ASyncTask::state_failed;			\
		return;										\
	}

	const int level = 0;

	bool commit = false;

	sync_.lock();
	state_ = ASyncTask::state_running;
	sync_.unlock();

	boost::system::error_code ec;

	std::vector<FileScanner::Path> srcFiles;

	FileScanner scanner(path_, ec);
	CHECK_EC(ec);

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
		int ret = scanner.nextFile(path, ec);
		CHECK_EC(ec);

		if (ret > 0) {
			srcFiles.push_back(path);
		}
		else if (ret < 0) {
			break;
		}
	}

	package_ = context_->uniquePath() + ".package";

	BOOST_SCOPE_EXIT_ALL(this, &commit)
	{
		if (!commit) {
			boost::system::error_code ec;
			if (fs::exists(package_, ec)) {
				fs::remove(package_, ec);
			}
		}
	};

	std::shared_ptr<Packer> packer(new Packer(package_, path_, srcFiles, level));

	sync_.lock();
	infoBody_ = "Packing";
	sync_.unlock();

	size_t lastIndex = -1;
	Packer::Path lastPackingFile;

	for (;;)
	{
		sync_.lock();
		if (cancelled_) {
			packer.reset();
			boost::system::error_code ec;
			fs::remove(package_, ec);
			infoBody_.clear();
			state_ = ASyncTask::state_cancelled;
			sync_.unlock();
			return;
		}
		sync_.unlock();

		const int ret = packer->executeStep();

		if (packer->currentIndex() != lastIndex) {
			lastIndex = packer->currentIndex();
			if (srcFiles.size()) {
				boost::mutex::scoped_lock lock(sync_);
				progress_ = (double)lastIndex / (double)srcFiles.size() * 100.0;
			}
		}

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

	commit = true;

	sync_.lock();
	infoBody_.clear();
	state_ = ASyncTask::state_finished;
	progress_ = 100;
	sync_.unlock();

#undef CHECK_EC
}

