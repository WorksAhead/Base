#ifndef BASECLIENT_HEADER_
#define BASECLIENT_HEADER_

#include <RpcSession.h>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

#include "Window.h"

#include <QTabWidget>

class BaseClient : public Window {
private:
	Q_OBJECT

public:
	BaseClient();
	~BaseClient();

	void setSession(Rpc::SessionPrx);

private:
	QTabWidget* tab_;

	Rpc::SessionPrx session_;
	IceUtil::TimerPtr timer_;
};

#endif // BASECLIENT_HEADER_

