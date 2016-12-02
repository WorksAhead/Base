#ifndef BASESERVER_CENTER_HEADER_
#define BASESERVER_CENTER_HEADER_

#include "SQLiteUtil.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/algorithm/string.hpp>

#include <unordered_map>
#include <vector>
#include <map>
#include <string>

typedef std::map<std::string, std::string> Form;

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
	void getGroupedCategories(std::map<std::string, std::string>&);

	bool lockEngineVersion(const std::string& name, const std::string& version, LockMode);
	void unlockEngineVersion(const std::string& name, const std::string& version, LockMode);

	bool lockClientVersion(const std::string& version, LockMode);
	void unlockClientVersion(const std::string& version, LockMode);

	std::string clientDir() const { return clientDir_; }
	std::string engineDir() const { return engineDir_; }
	std::string contentDir() const { return contentDir_; }
	std::string extraDir() const { return extraDir_; }

	std::string generateUuid();

	std::string getEnginePath(const std::string& name, const std::string& version);
	std::string getContentPath(const std::string& id);
	std::string getExtraPath(const std::string& id);
	std::string getClientPath(const std::string& version);

	bool addContent(const Form& form, const std::string& id);
	bool updateContent(const Form& form, const std::string& id);
	bool getContent(Form& form, const std::string& id);
	bool changeContentState(const std::string& id, const std::string& state);

	bool addEngineVersion(const std::string& name, const std::string& version, const Form& form);
	bool updateEngineVersion(const std::string& name, const std::string& version, const Form& form);
	bool getEngineVersion(const std::string& name, const std::string& version, Form& form);
	bool getEngineVersionState(const std::string& name, const std::string& version, std::string& outState);
	bool changeEngineVersionState(const std::string& name, const std::string& version, const std::string& state);

	bool addExtra(const Form& form, const std::string& id);
	bool updateExtra(const Form& form, const std::string& id);
	bool getExtra(Form& form, const std::string& id);
	bool changeExtraState(const std::string& id, const std::string& state);

	std::string getNewestClientVersion();
	bool addClientVersion(const std::string& version, const Form& form);
	bool updateClientVersion(const std::string& version, const Form& form);
	bool getClientVersion(const std::string& version, Form& form);
	bool getClientVersionState(const std::string& version, std::string& outState);
	bool changeClientVersionState(const std::string& version, const std::string& state);

	bool setUserGroup(const std::string& username, const std::string& group);
	bool removeUser(const std::string& username);

	DatabasePtr db() const { return db_; }

private:
	void loadPagesFromDb();
	void loadCategoriesFromDb();
	void updateCategoryGroup();

private:
	std::string baseDir_;
	std::string clientDir_;
	std::string engineDir_;
	std::string contentDir_;
	std::string extraDir_;

	boost::uuids::random_generator uniquePathGen_;

	DatabasePtr db_;

	std::vector<std::string> pages_;
	boost::mutex pagesSync_;

	std::vector<std::string> categories_;
	std::map<std::string, std::string> groupedCategories_;
	boost::mutex categoriesSync_;

	std::unordered_map<std::string, int> lockedEngineVersionSet_;
	boost::mutex lockedEngineVersionSetSync_;

	std::unordered_map<std::string, int> lockedClientVersionSet_;
	boost::mutex lockedClientVersionSetSync_;
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

class ClientVersionLockGuard {
public:
	ClientVersionLockGuard(Center* center, const std::string& version, Center::LockMode mode)
		: center_(center), version_(version), mode_(mode)
	{
		locked_ = center_->lockClientVersion(version_, mode_);
	}

	~ClientVersionLockGuard()
	{
		if (locked_) {
			center_->unlockClientVersion(version_, mode_);
		}
	}

	bool isLocked() const
	{
		return locked_;
	}

private:
	ClientVersionLockGuard(const ClientVersionLockGuard&);
	void operator=(const ClientVersionLockGuard&);

private:
	Center* center_;
	std::string version_;
	Center::LockMode mode_;
	bool locked_;
};

inline bool versionLess(const std::string& lhs, const std::string& rhs)
{
	std::vector<std::string> v1;
	boost::split(v1, lhs, boost::is_any_of("."));

	std::vector<std::string> v2;
	boost::split(v2, rhs, boost::is_any_of("."));

	for (int i = 0; i < 4; ++i)
	{
		int a = std::stoi(v1.at(i));
		int b = std::stoi(v2.at(i));

		if (a < b) {
			return true;
		}
		else if (a > b) {
			return false;
		}
	}

	return false;
}

typedef boost::shared_ptr<Center> CenterPtr;

inline void setEmptyIfNotExist(Form& form, const char* key)
{
	form.insert(std::make_pair(key, ""));
}

#endif // BASESERVER_CENTER_HEADER_

