#ifndef MANAGE_HEADER_
#define MANAGE_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration
class ManageEngine;
class ManagePage;
class ManageCategory;

class Manage : public QWidget {
private:
	Q_OBJECT

public:
	explicit Manage(ContextPtr, QWidget* parent = 0);
	~Manage();

private:
	ManageEngine* manageEngine_;
	ManagePage* managePage_;
	ManageCategory* manageCategory_;

	ContextPtr context_;
};

#endif // MANAGE_HEADER_

