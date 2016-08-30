#ifndef MANAGEEXTRAWIDGET_HEADER_
#define MANAGEEXTRAWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageExtraWidget.h"

class ManageExtraWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageExtraWidget(ContextPtr, QWidget* parent = 0);
	~ManageExtraWidget();

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
	Ui::ManageExtraWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::ExtraBrowserPrx browser_;
};

#endif // MANAGEEXTRAWIDGET_HEADER_

