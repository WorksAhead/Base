#ifndef MANAGEPAGEWIDGET_HEADER_
#define MANAGEPAGEWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManagePageWidget.h"

class ManagePageWidget : public QWidget {
private:
	Q_OBJECT

public:
	ManagePageWidget(ContextPtr context, QWidget* parent = 0);
	~ManagePageWidget();

protected:
	virtual void showEvent(QShowEvent*);

	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onRefresh();
	void onMoveUp();
	void onMoveDown();
	void onAdd();
	void onRemove();
	void onSubmit();

private:
	Ui::ManagePageWidget ui_;
	bool firstShow_;

	ContextPtr context_;
};

#endif // MANAGEPAGEWIDGET_HEADER_

