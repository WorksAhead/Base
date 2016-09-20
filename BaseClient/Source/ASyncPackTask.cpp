#include "AsyncPackTask.h"
#include "Package.h"
#include "FileScanner.h"
#include "ErrorMessage.h"
#include "Crc.h"

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <vector>
#include <sstream>

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

void ASyncPackTask::setSourceFiles(const std::string& srcFiles)
{
	srcFiles_ = srcFiles;
}

std::string ASyncPackTask::package()
{
	return package_;
}

void ASyncPackTask::start()
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

	{
		boost::mutex::scoped_lock lock(sync_);
		state_ = ASyncTask::state_running;
	}

	boost::system::error_code ec;	

	std::vector<FileScanner::Path> srcFiles;

	if (srcFiles_.empty())
	{
		FileScanner scanner(path_, ec);
		CHECK_EC(ec);

		{
			boost::mutex::scoped_lock lock(sync_);
			infoBody_ = "Scanning";
		}

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
	}
	else {
		std::istringstream is(srcFiles_);

		std::string line;
		while (std::getline(is, line)) {
			srcFiles.push_back(line);
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

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_ = "Packing";
	}

	size_t lastIndex = -1;
	Packer::Path lastPackingFile;

	for (;;)
	{
		{
			boost::mutex::scoped_lock lock(sync_);
			if (cancelled_) {
				packer.reset();
				boost::system::error_code ec;
				fs::remove(package_, ec);
				infoBody_.clear();
				state_ = ASyncTask::state_cancelled;
				return;
			}
		}

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

	{
		boost::mutex::scoped_lock lock(sync_);
		infoBody_.clear();
		state_ = ASyncTask::state_finished;
		progress_ = 100;
	}

#undef CHECK_EC
}

