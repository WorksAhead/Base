#ifndef PAGEEXTRAITEMWIDGET_HEADER_
#define PAGEEXTRAITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_PageExtraItemWidget.h"

class PageExtraItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageExtraItemWidget(ContextPtr context, QWidget* parent = 0);
	~PageExtraItemWidget();

	void setId(const QString& id);
	void setTitle(const QString& title);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onInstall();

private:
	ContextPtr context_;
	Ui::PageExtraItemWidget ui_;

	QString id_;
};

#endif // PAGEEXTRAITEMWIDGET_HEADER_

