#ifndef BASESERVER_CENTER_HEADER_
#define BASESERVER_CENTER_HEADER_

#include "SQLiteUtil.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/uuid/random_generator.hpp>

#include <unordered_map>
#include <vector>
#include <map>
#include <string>

class Center {
public:
	enum LockMode {
		lock_read,
		lock_write,
	};

public:
	Center();
	~Center();

	void setPages(const std::vector<std::string>&);
	void getPages(std::vector<std::string>&);

	void setCategories(const std::vector<std::string>&);
	void getCategories(std::vector<std::string>&);

	bool lockEngineVersion(const std::string& name, const std::string& version, LockMode);
	void unlockEngineVersion(const std::string& name, const std::string& version, LockMode);

	std::string engineDir() const { return engineDir_; }
	std::string contentDir() const { return contentDir_; }

	std::string generateUuid();

	std::string getEnginePath(const std::string& name, const std::string& version);
	std::string getContentPath(const std::string& uid);

	void addContent(const std::map<std::string, std::string>& form, const std::string& uid);
	bool getContent(std::map<std::string, std::string>& form, const std::string& uid);

	bool getEngineVersionState(const std::string& name, const std::string& version, std::string& outState);
	void addEngineVersion(const std::string& name, const std::string& version, const std::string& info);
	void changeEngineVersionState(const std::string& name, const std::string& version, const std::string& state);

	DatabasePtr db() const { return db_; }

private:
	void loadPagesFromDb();
	void loadCategoriesFromDb();

private:
	std::string baseDir_;
	std::string engineDir_;
	std::string contentDir_;

	boost::uuids::random_generator uniquePathGen_;

	DatabasePtr db_;

	std::vector<std::string> pages_;
	boost::mutex pagesSync_;

	std::vector<std::string> categories_;
	boost::mutex categoriesSync_;

	std::unordered_map<std::string, int> lockedEngineVersionSet_;
	boost::mutex lockedEngineVersionSetSync_;
};

class EngineVersionLockGuard {
public:
	EngineVersionLockGuard(Center* center, const std::string& name, const std::string& version, Center::LockMode mode)
		: center_(center), name_(name), version_(version), mode_(mode)
	{
		locked_ = center_->lockEngineVersion(name_, version_, mode_);
	}

	~EngineVersionLockGuard()
	{
		if (locked_) {
			center_->unlockEngineVersion(name_, version_, mode_);
		}
	}

	bool isLocked() const
	{
		return locked_;
	}

private:
	EngineVersionLockGuard(const EngineVersionLockGuard&);
	void operator=(const EngineVersionLockGuard&);

private:
	Center* center_;
	std::string name_;
	std::string version_;
	Center::LockMode mode_;
	bool locked_;
};

typedef boost::shared_ptr<Center> CenterPtr;

#endif // BASESERVER_CENTER_HEADER_

