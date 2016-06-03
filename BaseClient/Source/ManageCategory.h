#ifndef MANAGECATEGORY_HEADER_
#define MANAGECATEGORY_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManageCategory.h"

class ManageCategory : public QWidget {
private:
	Q_OBJECT

public:
	ManageCategory(ContextPtr context, QWidget* parent = 0);
	~ManageCategory();

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
	Ui::ManageCategory ui_;
	bool firstShow_;

	ContextPtr context_;
};

#endif // MANAGECATEGORY_HEADER_

