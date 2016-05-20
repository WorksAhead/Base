#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

#include "Window.h"

#include <QTabWidget>

// forward declaration
class Manage;

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	explicit BaseClient(Rpc::SessionPrx);
	~BaseClient();

private:
	QTabWidget* tab_;
	Manage* manage_;

	Rpc::SessionPrx session_;
	IceUtil::TimerPtr timer_;
};

#endif // BASECLIENT_HEADER_

