#ifndef MANAGEENGINEWIDGET_HEADER_
#define MANAGEENGINEWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageEngineWidget.h"

class ManageEngineWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageEngineWidget(ContextPtr, QWidget* parent = 0);
	~ManageEngineWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShowMore();
	void onShowAll();
	void onRefresh();
	void onEdit();
	void onRemove();
	void showSubmitDialog();

private:
	void showMore(int);

private:
	Ui::ManageEngineWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::EngineVersionBrowserPrx browser_;
};

#endif // MANAGEENGINEWIDGET_HEADER_

