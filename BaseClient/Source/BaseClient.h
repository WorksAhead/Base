#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include "Context.h"

#include "Window.h"
#include "ui_DecoratorWidget.h"
#include "ASyncTaskManagerDialog.h"

#include <QTabWidget>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

// forward declaration
class Manage;

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	explicit BaseClient(Rpc::SessionPrx);
	~BaseClient();

private:
	Ui::DecoratorWidget decoratorWidgetUi_;

	ASyncTaskManagerDialog* taskManagerDialog_;

	QTabWidget* tab_;
	Manage* manage_;

	ContextPtr context_;
	IceUtil::TimerPtr timer_;
};

#endif // BASECLIENT_HEADER_

