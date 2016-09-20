#ifndef PACKAGE_HEADER_
#define PACKAGE_HEADER_

#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <memory>

class Packer {
private:
	enum {
		state_open_package,
		state_next_file,
		state_packing,
		state_failed,
		state_finished,
	};

public:
	typedef boost::filesystem::path Path;

public:
	Packer(const Path& packFile, const Path& basePath, const std::vector<Path>& srcFiles, int level);
	~Packer();

	size_t currentIndex() const;
	Path currentFile() const;
	std::string errorMessage() const;

	int executeStep();

private:
	Path packFile_;
	Path basePath_;
	std::vector<Path> srcFiles_;
	int level_;
	void* handle_;
	int state_;
	size_t currentIdx_;
	std::shared_ptr<std::fstream> stream_;
	std::streampos fileLen_;
	std::streamsize remain_;
	std::vector<char> buf_;
	std::string errorMessage_;
};

class Unpacker {
private:
	enum {
		state_open_package,
		state_next_file,
		state_unpacking,
		state_failed,
		state_finished,
	};

public:
	typedef boost::filesystem::path Path;

public:
	Unpacker(const Path& packFile, const Path& outPath);
	~Unpacker();

	size_t count() const;
	size_t currentIndex() const;
	Path currentFile() const;
	std::string errorMessage() const;

	int executeStep();

private:
	bool isDirectory(const Path&);

private:
	Path packFile_;
	Path outPath_;
	void* handle_;
	int state_;
	size_t count_;
	size_t currentIdx_;
	Path currentFile_;
	std::shared_ptr<std::fstream> stream_;
	std::vector<char> buf_;
	std::string errorMessage_;
};

#endif // PACKAGE_HEADER_

