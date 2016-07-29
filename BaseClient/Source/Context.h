#ifndef CONTEXT_HEADER_
#define CONTEXT_HEADER_

#include "ASyncTask.h"

#include <RpcSession.h>

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

struct ProjectInfo {
	std::string id;
	std::string contentId;
	std::string location;
	std::string name;
};

struct Context
{
	Rpc::SessionPrx session;

	ContentImageLoader* contentImageLoader;

	std::function<void(ASyncTaskPtr)> addTask;

	std::function<void()> showTaskManager;

	std::function<std::string()> uniquePath;
	std::function<std::string()> cachePath;
	std::function<std::string()> libraryPath;

	std::function<std::string(const std::string&, const std::string&)> enginePath;
	std::function<std::string(const std::string&)> contentPath;

	std::function<void(const std::string&, const std::string&)> installEngine;
	std::function<int(const std::string&, const std::string&)> getEngineState;
	std::function<bool(const std::string&, const std::string&, int&, int)> changeEngineState;

	std::function<void(std::vector<std::string>&)> getDownloadedContentList;

	std::function<int(const std::string&)> getContentState;
	std::function<bool(const std::string&, int&, int)> changeContentState;

	std::function<void(const std::string&, const std::string&, const std::string&)> createProject;
	std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)> addProject;
	std::function<void(const std::string&, bool)> removeProject;
	std::function<bool(ProjectInfo&, const std::string& id)> getProject;
	std::function<void(std::vector<ProjectInfo>&)> getProjectList;

	std::function<void(Rpc::ErrorCode)> promptRpcError;
};

typedef std::shared_ptr<Context> ContextPtr;

#endif // CONTEXT_HEADER_

