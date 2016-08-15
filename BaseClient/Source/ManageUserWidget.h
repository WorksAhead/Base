#ifndef MANAGEUSERWIDGET_HEADER_
#define MANAGEUSERWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageUserWidget.h"

class ManageUserWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageUserWidget(ContextPtr, QWidget* parent = 0);
	~ManageUserWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShowMore();
	void onShowAll();
	void onRefresh();
	void onResetPassword();
	void onRemove();

private:
	void showMore(int);

private:
	Ui::ManageUserWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::UserBrowserPrx browser_;
};

#endif // MANAGEUSERWIDGET_HEADER_

