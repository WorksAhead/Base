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
	std::string libraryPath();

	std::string enginePath(const std::string& name, const std::string& version);
	std::string contentPath(const std::string& id);

	void installEngine(const std::string& name, const std::string& version);
	int getEngineState(const std::string& name, const std::string& version);
	bool changeEngineState(const std::string& name, const std::string& version, int& oldState, int newState);

	void getDownloadedContentList(std::vector<std::string>&);

	int getContentState(const std::string& id);
	bool changeContentState(const std::string& id, int& oldState, int newState);

	void promptRpcError(Rpc::ErrorCode);

private Q_SLOTS:
	void onShowTaskManager();

private:
	void initDb();
	void loadDownloadedContentsFromDb();
	void loadInstalledEnginesFromDb();

private:
	Ui::DecoratorWidget decoratorWidgetUi_;

	DatabasePtr db_;

	ASyncTaskManagerDialog* taskManagerDialog_;

	LowerPaneWidget* lowerPane_;
	QTabWidget* tabWidget_;
	LibraryWidget* library_;
	ManageWidget* manage_;

	ContextPtr context_;

	IceUtil::TimerPtr timer_;

	boost::uuids::random_generator uniquePathGen_;

	std::unordered_set<std::string> downloadedContentTabel_;
	boost::recursive_mutex downloadedContentTabelSync_;

	std::unordered_set<std::string> installedEngineTabel_;
	boost::recursive_mutex installedEngineTabelSync_;

	std::unordered_map<std::string, int> contentStateTabel_;
	boost::recursive_mutex contentStateTabelSync_;

	std::unordered_map<std::string, int> engineStateTabel_;
	boost::recursive_mutex engineStateTabelSync_;
};

#endif // BASECLIENT_HEADER_

