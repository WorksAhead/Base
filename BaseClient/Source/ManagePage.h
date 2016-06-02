#ifndef MANAGEPAGE_HEADER_
#define MANAGEPAGE_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_ManagePage.h"

class ManagePage : public QWidget {
private:
	Q_OBJECT

public:
	ManagePage(ContextPtr context, QWidget* parent = 0);
	~ManagePage();

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
	Ui::ManagePage ui_;
	bool firstShow_;

	ContextPtr context_;
};

#endif // MANAGEPAGE_HEADER_

