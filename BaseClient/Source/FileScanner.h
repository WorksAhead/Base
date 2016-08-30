#ifndef FILESCANNER_HEADER_
#define FILESCANNER_HEADER_

#include "PathUtils.h"

#include <boost/filesystem.hpp>

#include <list>

class FileScanner {
public:
	typedef boost::filesystem::path Path;
	typedef boost::filesystem::directory_iterator DirectoryIterator;

public:
	FileScanner(const Path& path, boost::system::error_code& ec) : stack_(1, DirectoryIterator(makeSafePath(path), ec))
	{
	}

	int nextFile(Path& outPath, boost::system::error_code& ec)
	{
		DirectoryIterator& current = stack_.back();

		if (current != DirectoryIterator())
		{
			const Path& safePath = makeSafePath(current->path());

			if (boost::filesystem::is_regular_file(safePath, ec))
			{
				outPath = base_ / safePath.filename();
				++current;
				return 1;
			}
			else if (boost::filesystem::is_directory(safePath, ec))
			{
				stack_.push_back(DirectoryIterator(makeSafePath(safePath)));
				try {
					base_ = base_ / safePath.leaf();
				}
				catch (...) {
					stack_.pop_back();
					throw;
				}
				++current;
				return 0;
			}
			else {
				++current;
				return 0;
			}
		}
		else if (stack_.size() > 1)
		{
			base_ = base_.parent_path();
			stack_.pop_back();
			return 0;
		}
		else {
			return -1;
		}
	}

private:
	Path base_;
	std::list<DirectoryIterator> stack_;
};

#endif // FILESCANNER_HEADER_

