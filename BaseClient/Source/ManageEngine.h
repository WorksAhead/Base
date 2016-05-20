#ifndef MANAGEENGINE_HEADER_
#define MANAGEENGINE_HEADER_

#include "ui_ManageEngine.h"

#include <RpcSession.h>

class ManageEngine : public QWidget {
private:
	Q_OBJECT

public:
	explicit ManageEngine(Rpc::SessionPrx, QWidget* parent = 0);
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

	Rpc::SessionPrx session_;
	Rpc::EngineBrowserPrx browser_;
	Rpc::EngineUploaderPrx engineUploader_; // test
};

#endif // MANAGEENGINE_HEADER_

