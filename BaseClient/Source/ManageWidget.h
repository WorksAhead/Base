#ifndef MANAGEWIDGET_HEADER_
#define MANAGEWIDGET_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration
class ManageContentWidget;
class ManageEngineWidget;
class ManageExtraWidget;
class ManageUserWidget;
class ManageInfoWidget;
class ManageClientWidget;

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
	ManageExtraWidget* manageExtra_;
	ManageUserWidget* manageUser_;
	ManageInfoWidget* manageInfoWidget_;
	ManageClientWidget* manageClient_;

	ContextPtr context_;
};

#endif // MANAGEWIDGET_HEADER_

