#ifndef MANAGEENGINE_HEADER_
#define MANAGEENGINE_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageEngine.h"

class ManageEngine : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageEngine(ContextPtr, QWidget* parent = 0);
	~ManageEngine();

protected:
	virtual void showEvent(QShowEvent*);

	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShowMore();
	void onShowAll();
	void onRefresh();
	void onRemove();
	void showSubmitDialog();

private:
	void showMore(int);

private:
	Ui::ManageEngine ui_;
	bool firstShow_;

	ContextPtr context_;

	Rpc::EngineVersionBrowserPrx browser_;
};

#endif // MANAGEENGINE_HEADER_

