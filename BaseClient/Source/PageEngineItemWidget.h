#ifndef PAGEENGINEITEMWIDGET_HEADER_
#define PAGEENGINEITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_PageEngineItemWidget.h"

class PageEngineItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageEngineItemWidget(ContextPtr context, QWidget* parent = 0);
	~PageEngineItemWidget();

	void setEngineVersion(const QPair<QString, QString>& v);
	QPair<QString, QString> getEngineVersion();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onInstall();

private:
	ContextPtr context_;
	Ui::PageEngineItemWidget ui_;

	QPair<QString, QString> engineVersion_;
};

#endif // PAGEENGINEITEMWIDGET_HEADER_

