#ifndef MANAGE_HEADER_
#define MANAGE_HEADER_

#include <QWidget>

#include <RpcSession.h>

// forward declaration
class ManageEngine;

class Manage : public QWidget {
private:
	Q_OBJECT

public:
	explicit Manage(Rpc::SessionPrx, QWidget* parent = 0);
	~Manage();

private:
	ManageEngine* manageEngine_;

	Rpc::SessionPrx session_;
};

#endif // MANAGE_HEADER_

