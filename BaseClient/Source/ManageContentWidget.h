#ifndef MANAGECONTENTWIDGET_HEADER_
#define MANAGECONTENTWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageContentWidget.h"

class ManageContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageContentWidget(ContextPtr, QWidget* parent = 0);
	~ManageContentWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShowMore();
	void onShowAll();
	void onRefresh();
	void onEdit();
	void onRemove();

private:
	void showMore(int);

private:
	Ui::ManageContentWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::ContentBrowserPrx browser_;
};

#endif // MANAGECONTENTWIDGET_HEADER_

