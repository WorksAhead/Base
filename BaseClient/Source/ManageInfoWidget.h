#ifndef MANAGEINFOWIDGET_HEADER_
#define MANAGEINFOWIDGET_HEADER_

#include "Context.h"

#include "ui_ManageInfoWidget.h"

class ManageInfoWidget : public QWidget {
private:
	Q_OBJECT

public:
	ManageInfoWidget(ContextPtr context, QWidget* parent = 0);
	~ManageInfoWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onRefresh();
	void onEdit();

private:
	ContextPtr context_;

	Ui::ManageInfoWidget ui_;

	bool firstShow_;
};

#endif // MANAGEINFOWIDGET_HEADER_

