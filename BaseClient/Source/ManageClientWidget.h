#ifndef MANAGECLIENTWIDGET_HEADER_
#define MANAGECLIENTWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageClientWidget.h"

class ManageClientWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageClientWidget(ContextPtr, QWidget* parent = 0);
	~ManageClientWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShowMore();
	void onShowAll();
	void onRefresh();
	void onSubmit();
	void onEdit();
	void onRemove();

private:
	void showMore(int);

private:
	Ui::ManageClientWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::ClientBrowserPrx browser_;
};

#endif // MANAGECLIENTWIDGET_HEADER_

