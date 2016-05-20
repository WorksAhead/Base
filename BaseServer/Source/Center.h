#ifndef BASESERVER_CENTER_HEADER_
#define BASESERVER_CENTER_HEADER_

#include "SQLiteUtil.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <unordered_map>
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

	bool lockEngineVersion(const std::string& name, const std::string& version, LockMode);
	void unlockEngineVersion(const std::string& name, const std::string& version, LockMode);

	std::string baseDir() const { return baseDir_; }
	std::string engineDir() const { return engineDir_; }

	std::string engineFileName(const std::string& name, const std::string& version) const;

	bool getEngineVersionState(const std::string& name, const std::string& version, std::string& outState);
	void addEngineVersion(const std::string& name, const std::string& version, const std::string& info);
	void changeEngineVersionState(const std::string& name, const std::string& version, const std::string& state);

	DatabasePtr db() const { return db_; }

private:
	std::string baseDir_;
	std::string engineDir_;
	DatabasePtr db_;
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

