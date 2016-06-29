#include "Package.h"

#include <zip.h>
#include <unzip.h>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <locale>
#include <codecvt>
#include <memory.h>

namespace fs = boost::filesystem;

Packer::Packer(const Path& packFile, const Path& basePath, const std::vector<Path>& srcFiles, int level)
	: packFile_(packFile), basePath_(basePath), srcFiles_(srcFiles), level_(level)
{
	state_ = state_open_package;
	currentIdx_ = 0;
}

Packer::~Packer()
{
	if (handle_) {
		if (state_ == state_packing) {
			zipCloseFileInZip(handle_);
		}
		zipClose(handle_, 0);
	}
}

size_t Packer::currentIndex() const
{
	return currentIdx_;
}

Packer::Path Packer::currentFile() const
{
	if (currentIdx_ < srcFiles_.size()) {
		return srcFiles_[currentIdx_];
	}
	else {
		return "";
	}
}

std::string Packer::errorMessage() const
{
	return errorMessage_;
}

int Packer::executeStep()
{
	if (state_ == state_finished) {
		return 0;
	}
	else if (state_ == state_failed) {
		return -1;
	}

	if (state_ == state_open_package)
	{
		handle_ = zipOpen(packFile_.string().c_str(), APPEND_STATUS_CREATE);
		if (!handle_) {
			errorMessage_ = "Failed to create package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		state_ = state_next_file;
	}
	else if (state_ == state_next_file)
	{
		if (currentIdx_ == srcFiles_.size())
		{
			if (zipClose(handle_, 0) != ZIP_OK) {
				errorMessage_ = "Write error";
				state_ = state_failed;
				return -1;
			}
			handle_ = 0;
			state_ = state_finished;
			return 0;
		}

		Path path = srcFiles_[currentIdx_];
		Path fullPath = basePath_ / path;

		stream_.reset(new std::fstream(fullPath.string().c_str(), std::ios::in|std::ios::binary));
		if (!stream_->is_open()) {
			errorMessage_ = "Failed to open \"" + fullPath.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		stream_->seekg(0, std::ios::end);
		fileLen_ = stream_->tellg();
		stream_->seekg(0, std::ios::beg);

		//if (fileLen_ > 0xffffffff) {
		//	errorMessage_ = "File is too large \"" + fullPath.string() + "\"";
		//	state_ = state_failed;
		//	return -1;
		//}

		zip_fileinfo zipFileInfo;
		{
			memset(&zipFileInfo, 0, sizeof(zip_fileinfo));
			time_t lastWriteTime = fs::last_write_time(fullPath);
			struct tm* ti = localtime(&lastWriteTime);
			zipFileInfo.tmz_date.tm_year = ti->tm_year + 1900;
			zipFileInfo.tmz_date.tm_mon = ti->tm_mon;
			zipFileInfo.tmz_date.tm_mday = ti->tm_mday;
			zipFileInfo.tmz_date.tm_hour = ti->tm_hour;
			zipFileInfo.tmz_date.tm_min = ti->tm_min;
			zipFileInfo.tmz_date.tm_sec = ti->tm_sec;
		}

		if (zipOpenNewFileInZip(handle_, path.string().c_str(), &zipFileInfo, 0, 0, 0, 0, 0, Z_DEFLATED, level_) != ZIP_OK) {
			errorMessage_ = "Write error";
			state_ = state_failed;
			return -1;
		}

		remain_ = fileLen_;

		state_ = state_packing;
		return 1;
	}
	else if (state_ == state_packing)
	{
		if (remain_ > 0)
		{
			if (buf_.size() == 0) {
				buf_.resize(1024*1024);
			}

			std::streamsize n = std::min<std::streamsize>(buf_.size(), remain_);

			if (!stream_->read(&buf_[0], n)) {
				errorMessage_ = "Read error";
				state_ = state_failed;
				return -1;
			}

			if (zipWriteInFileInZip(handle_, &buf_[0], n) < 0) {
				errorMessage_ = "Write error";
				state_ = state_failed;
				return -1;
			}

			remain_ -= n;
		}

		if (remain_ == 0)
		{
			if (zipCloseFileInZip(handle_) != ZIP_OK) {
				errorMessage_ = "Write error";
				state_ = state_failed;
				return -1;
			}

			stream_.reset();
			++currentIdx_;
			state_ = state_next_file;
		}
	}

	return 1;
}

