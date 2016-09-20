#include "Package.h"
#include "PathUtils.h"

#include <zip.h>
#include <unzip.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

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
		std::string safePackFile = makeSafePath(packFile_);
		handle_ = zipOpen(safePackFile.c_str(), APPEND_STATUS_CREATE);
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
		Path safeFullPath = makeSafePath(basePath_ / path);
		
		stream_.reset(new std::fstream(safeFullPath.c_str(), std::ios::in|std::ios::binary));
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
			time_t lastWriteTime = fs::last_write_time(safeFullPath);
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

			if (!stream_->read(buf_.data(), n)) {
				errorMessage_ = "Read error";
				state_ = state_failed;
				return -1;
			}

			if (zipWriteInFileInZip(handle_, buf_.data(), n) < 0) {
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

Unpacker::Unpacker(const Path& packFile, const Path& outPath) : packFile_(packFile), outPath_(outPath)
{
	state_ = state_open_package;
	count_ = 0;
	currentIdx_ = 0;
}

Unpacker::~Unpacker()
{
	if (handle_) {
		if (state_ == state_unpacking) {
			// todo
		}
		unzClose(handle_);
	}
}

size_t Unpacker::count() const
{
	return count_;
}

size_t Unpacker::currentIndex() const
{
	return currentIdx_;
}

Unpacker::Path Unpacker::currentFile() const
{
	return currentFile_;
}

std::string Unpacker::errorMessage() const
{
	return errorMessage_;
}

int Unpacker::executeStep()
{
	if (state_ == state_finished) {
		return 0;
	}
	else if (state_ == state_failed) {
		return -1;
	}

	if (state_ == state_open_package)
	{
		std::string safePackFile = makeSafePath(packFile_);
		handle_ = unzOpen(safePackFile.c_str());
		if (!handle_) {
			errorMessage_ = "Failed to open package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		if (unzGoToFirstFile(handle_) != UNZ_OK) {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		for (;;)
		{
			++count_;
			const int ret = unzGoToNextFile(handle_);
			if (ret == UNZ_OK) {
				continue;
			}
			else if (ret == UNZ_END_OF_LIST_OF_FILE) {
				break;
			}
			else {
				errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
				state_ = state_failed;
				return -1;
			}
		}

		if (unzGoToFirstFile(handle_) != UNZ_OK) {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		state_ = state_next_file;
	}
	else if (state_ == state_next_file)
	{
		unz_file_info fileInfo;

		if (unzGetCurrentFileInfo(handle_, &fileInfo, 0, 0, 0, 0, 0, 0) != UNZ_OK) {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		if (buf_.size() < fileInfo.size_filename) {
			buf_.resize(fileInfo.size_filename);
		}

		if (unzGetCurrentFileInfo(handle_, 0, buf_.data(), fileInfo.size_filename, 0, 0, 0, 0) != UNZ_OK) {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		Path path(buf_.data(), buf_.data() + fileInfo.size_filename);
		Path fullPath = outPath_ / path;
		Path safeFullPath = makeSafePath(outPath_ / path);

		if (!fs::exists(safeFullPath.parent_path())) {
			boost::system::error_code ec;
			if (!fs::create_directories(safeFullPath.parent_path(), ec)) {
				errorMessage_ = "Failed to create directory \"" + fullPath.parent_path().string() + "\"";
				state_ = state_failed;
				return -1;
			}
		}

		if (isDirectory(path))
		{
			const int ret = unzGoToNextFile(handle_);

			if (ret == UNZ_OK) {
				++currentIdx_;
				state_ = state_next_file;
			}
			if (ret == UNZ_END_OF_LIST_OF_FILE) {
				unzClose(handle_);
				handle_ = 0;
				state_ = state_finished;
				return 0;
			}
			else if (ret != UNZ_OK) {
				errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
				state_ = state_failed;
				return -1;
			}

			return 1;
		}

		stream_.reset(new std::fstream(safeFullPath.string().c_str(), std::ios::out|std::ios::binary));
		if (!stream_->is_open()) {
			errorMessage_ = "Failed to open \"" + fullPath.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		if (unzOpenCurrentFile(handle_) != UNZ_OK) {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}

		currentFile_ = path;
		state_ = state_unpacking;
	}
	else if (state_ == state_unpacking)
	{
		if (buf_.size() < 1024*1024) {
			buf_.resize(1024*1024);
		}

		const int n = unzReadCurrentFile(handle_, buf_.data(), buf_.size());

		if (n > 0)
		{
			if (!stream_->write(buf_.data(), n)) {
				errorMessage_ = "Write error";
				state_ = state_failed;
				return -1;
			}
		}
		else if (n == 0)
		{
			stream_.reset();

			if (unzCloseCurrentFile(handle_) != UNZ_OK) {
				errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
				state_ = state_failed;
				return -1;
			}

			const int ret = unzGoToNextFile(handle_);

			if (ret == UNZ_OK) {
				++currentIdx_;
				state_ = state_next_file;
			}
			if (ret == UNZ_END_OF_LIST_OF_FILE) {
				unzClose(handle_);
				handle_ = 0;
				state_ = state_finished;
				return 0;
			}
			else if (ret != UNZ_OK) {
				errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
				state_ = state_failed;
				return -1;
			}
		}
		else {
			errorMessage_ = "Bad package file \"" + packFile_.string() + "\"";
			state_ = state_failed;
			return -1;
		}
	}

	return 1;
}

bool Unpacker::isDirectory(const Path& p)
{
	const std::string& s = p.string();
	return (boost::ends_with(s, "/") || boost::ends_with(s, "\\"));
}

