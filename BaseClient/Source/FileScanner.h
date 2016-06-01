#ifndef FILESCANNER_HEADER_
#define FILESCANNER_HEADER_

#include <boost/filesystem.hpp>

#include <list>

class FileScanner {
public:
	typedef boost::filesystem::path Path;
	typedef boost::filesystem::directory_iterator DirectoryIterator;

public:
	explicit FileScanner(const Path& path) : stack_(1, DirectoryIterator(path))
	{
	}

	int nextFile(Path& outPath)
	{
		DirectoryIterator& current = stack_.back();

		if (current != DirectoryIterator())
		{
			const Path& path = current->path();

			if (boost::filesystem::is_regular_file(path))
			{
				outPath = base_ / path.filename();
				++current;
				return 1;
			}
			else if (boost::filesystem::is_directory(path))
			{
				stack_.push_back(DirectoryIterator(path));
				try {
					base_ = base_ / path.leaf();
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

