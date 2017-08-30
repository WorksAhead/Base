#ifndef MANAGECONTENTWIDGET_HEADER_
#define MANAGECONTENTWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageContentWidget.h"

#include <QStringList>

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
	void onSubmit();
	void onEdit();

private:
	void showMore(int);
	QStringList queryEngineVersions(bool refresh = false);

private:
	Ui::ManageContentWidget ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::ContentBrowserPrx browser_;

	QStringList cachedEngineVersions_;
};

#endif // MANAGECONTENTWIDGET_HEADER_

