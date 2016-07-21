#ifndef MANAGECATEGORYWIDGET_HEADER_
#define MANAGECATEGORYWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageCategoryWidget.h"

class ManageCategoryWidget : public QWidget {
private:
	Q_OBJECT

public:
	ManageCategoryWidget(ContextPtr context, QWidget* parent = 0);
	~ManageCategoryWidget();

protected:
	virtual void showEvent(QShowEvent*);

	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onRefresh();
	void onMoveUp();
	void onMoveDown();
	void onAdd();
	void onRemove();
	void onSave();

private:
	Ui::ManageCategoryWidget ui_;
	bool firstShow_;

	ContextPtr context_;
};

#endif // MANAGECATEGORYWIDGET_HEADER_

