#ifndef PAGEENGINEWIDGET_HEADER_
#define PAGEENGINEWIDGET_HEADER_

#include "ui_PageEngineWidget.h"

#include <RpcSession.h>

#include "Context.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTimer>

// forward declaration
class PageEngineItemWidget;
class FlowLayout;

class PageEngineWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageEngineWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageEngineWidget();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onScroll(int);
	void onRefresh();

private:
	void showMore(int);
	void clear();

private:
	ContextPtr context_;
	QString name_;
	FlowLayout* flowLayout_;

	Rpc::EngineVersionBrowserPrx browser_;

	Ui::PageEngineWidget ui_;
	bool firstShow_;
};

#endif // PAGEENGINEWIDGET_HEADER_

