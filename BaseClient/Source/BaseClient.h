#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include "Context.h"
#include "SQLiteUtil.h"

#include "Window.h"
#include "ui_DecoratorWidget.h"
#include "LowerPaneWidget.h"
#include "ASyncTaskManagerDialog.h"

#include <QTabWidget>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/uuid/random_generator.hpp>

#include <unordered_set>
#include <unordered_map>
#include <map>

// forward declaration
class LibraryWidget;
class ManageWidget;

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	explicit BaseClient(Rpc::SessionPrx);
	~BaseClient();

private:
	void addTask(ASyncTaskPtr);
	
	std::string uniquePath();
	std::string cachePath();
	std::string userPath();
	std::string libraryPath();
	std::string outputPath();

	std::string enginePath(const EngineVersion&);
	std::string contentPath(const std::string& id);

	void installEngine(const EngineVersion&);
	void setupEngine(const EngineVersion&);
	void unSetupEngine(const EngineVersion&);
	void removeEngine(const EngineVersion&);
	int getEngineState(const EngineVersion&);
	bool changeEngineState(const EngineVersion&, int& oldState, int newState);
	void getEngineList(std::vector<EngineVersion>&);

	void getDownloadedContentList(std::vector<std::string>&);

	int getContentState(const std::string& id);
	bool changeContentState(const std::string& id, int& oldState, int newState);

	void createProject(const std::string& contentId, const std::string& title, const std::string& location);
	void addProject(const std::string& id, const std::string& contentId, const std::string& location, const std::map<std::string, std::string>& properties);
	void removeProject(const std::string& id, bool removeDir);
	void renameProject(const std::string& id, const std::string& newName);
	bool getProject(ProjectInfo&, const std::string& id);
	void getProjectList(std::vector<ProjectInfo>&);

private Q_SLOTS:
	void addEngineToGui(const EngineVersion&);
	void removeEngineFromGui(const EngineVersion&);
	void addContentToGui(const std::string& contentId);
	void removeContentFromGui(const std::string& contentId);
	void addProjectToGui(const std::string& projectId);
	void removeProjectFromGui(const std::string& projectId);
	void prompt(int level, const std::string& message);
	void promptRpcError(Rpc::ErrorCode);
	void promptEngineState(const EngineVersion&, int);
	void onShowTaskManager();

private:
	void initDb();
	void loadDownloadedContentsFromDb();
	void loadInstalledEnginesFromDb();
	void loadProjectsFromDb();

private:
	Ui::DecoratorWidget decoratorWidgetUi_;

	IceUtil::TimerPtr timer_;

	DatabasePtr db_;

	ContextPtr context_;

	ASyncTaskManagerDialog* taskManagerDialog_;

	LowerPaneWidget* lowerPane_;
	QTabWidget* tabWidget_;
	LibraryWidget* library_;
	ManageWidget* manage_;

	boost::uuids::random_generator rand_;

	std::unordered_set<std::string> downloadedContentTabel_;
	boost::recursive_mutex downloadedContentTabelSync_;

	std::unordered_set<EngineVersion, EngineVersionHash, EngineVersionEq> installedEngineTabel_;
	boost::recursive_mutex installedEngineTabelSync_;

	std::unordered_map<std::string, int> contentStateTabel_;
	boost::recursive_mutex contentStateTabelSync_;

	std::unordered_map<EngineVersion, int, EngineVersionHash, EngineVersionEq> engineStateTabel_;
	boost::recursive_mutex engineStateTabelSync_;

	std::unordered_map<std::string, ProjectInfo> projectTabel_;
	boost::recursive_mutex projectTabelSync_;
};

#endif // BASECLIENT_HEADER_

