#ifndef CONTEXT_HEADER_
#define CONTEXT_HEADER_

#include "ASyncTask.h"

#include <RpcSession.h>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>

#include <functional>
#include <memory>
#include <string>

// forward declaration
class ContentImageLoader;

namespace EngineState {
	enum State {
		not_installed = 0,
		installing,
		installed,
		configuring,
		removing,
	};
}

namespace ContentState {
	enum State {
		not_downloaded = 0,
		downloading,
		downloaded,
	};
}

namespace ExtraState {
	enum State {
		not_downloaded = 0,
		downloading,
		downloaded,
		configuring,
	};
}

struct ProjectInfo {
	std::string id;
	std::string contentId;
	std::string location;
	std::string name;
	std::string defaultEngineVersion;
	std::string startup;
};

typedef std::pair<std::string, std::string> EngineVersion;

class EngineVersionEq : public std::binary_function<EngineVersion, EngineVersion, bool> {
public:
	bool operator()(const EngineVersion& a, const EngineVersion& b) const
	{
		std::locale locale;
		return (boost::iequals(a.first, b.first, locale) && boost::iequals(a.second, b.second, locale));
	}
};

class EngineVersionHash : public std::unary_function<EngineVersion, std::size_t> {
public:
	std::size_t operator()(const EngineVersion& v) const
	{
		std::size_t seed = 0;
		std::locale locale;

		for (auto ch : v.first) {
			boost::hash_combine(seed, std::tolower(ch, locale));
		}
		for (auto ch : v.second) {
			boost::hash_combine(seed, std::tolower(ch, locale));
		}

		return seed;
	}
};

struct Context
{
	Rpc::SessionPrx session;

	std::string currentUser;
	std::string currentUserGroup;

	ContentImageLoader* contentImageLoader;

	std::function<void(ASyncTaskPtr)> addTask;

	std::function<void()> showTaskManager;

	std::function<std::string()> uniquePath;
	std::function<std::string()> cachePath;
	std::function<std::string()> libraryPath;

	std::function<std::string(const EngineVersion&)> enginePath;
	std::function<std::string(const std::string&)> contentPath;
	std::function<std::string(const std::string&)> extraPath;

	std::function<void(const EngineVersion&)> installEngine;
	std::function<void(const EngineVersion&)> setupEngine;
	std::function<void(const EngineVersion&)> unSetupEngine;
	std::function<void(const EngineVersion&)> removeEngine;
	std::function<int(const EngineVersion&)> getEngineState;
	std::function<bool(const EngineVersion&, int&, int)> changeEngineState;
	std::function<void(std::vector<EngineVersion>&)> getEngineList;
	std::function<void(const EngineVersion&)> addEngineToGui;
	std::function<void(const EngineVersion&)> removeEngineFromGui;

	std::function<void(std::vector<std::string>&)> getDownloadedContentList;
	std::function<int(const std::string&)> getContentState;
	std::function<bool(const std::string&, int&, int)> changeContentState;
	std::function<void(const std::string&)> addContentToGui;
	std::function<void(const std::string&)> removeContentFromGui;

	std::function<void(std::vector<std::string>&)> getDownloadedExtraList;
	std::function<void(const std::string&)> installExtra;
	std::function<void(const std::string&)> setupExtra;
	std::function<int(const std::string&)> getExtraState;
	std::function<bool(const std::string&, int&, int)> changeExtraState;
	std::function<void(const std::string&)> addExtraToGui;
	std::function<void(const std::string&)> removeExtraFromGui;

	std::function<void(const std::string&, const std::string&, const std::string&)> createProject;
	std::function<void(const std::string&, const std::string&, const std::string&, const std::map<std::string, std::string>& properties)> addProject;
	std::function<void(const std::string&, bool)> removeProject;
	std::function<void(const std::string&, const std::string&)> renameProject;
	std::function<bool(ProjectInfo&, const std::string& id)> getProject;
	std::function<void(std::vector<ProjectInfo>&)> getProjectList;
	std::function<void(const std::string&)> addProjectToGui;
	std::function<void(const std::string&)> removeProjectFromGui;

	std::function<void(int, const std::string&)> prompt;
	std::function<void(Rpc::ErrorCode)> promptRpcError;
	std::function<void(const EngineVersion&, int)> promptEngineState;
	std::function<void(const std::string&, int)> promptExtraState;
};

typedef std::shared_ptr<Context> ContextPtr;

#endif // CONTEXT_HEADER_

