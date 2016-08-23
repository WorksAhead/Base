#ifndef MANAGEWIDGET_HEADER_
#define MANAGEWIDGET_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration
class ManageContentWidget;
class ManageEngineWidget;
class ManageUserWidget;
class ManagePageWidget;
class ManageCategoryWidget;

class ManageWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageWidget(ContextPtr, QWidget* parent = 0);
	~ManageWidget();

protected:
	virtual void paintEvent(QPaintEvent*);

private:
	ManageContentWidget* manageContent_;
	ManageEngineWidget* manageEngine_;
	ManageUserWidget* manageUser_;
	ManagePageWidget* managePage_;
	ManageCategoryWidget* manageCategory_;

	ContextPtr context_;
};

#endif // MANAGEWIDGET_HEADER_

