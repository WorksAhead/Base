#ifndef PAGEENGINEWIDGET_HEADER_
#define PAGEENGINEWIDGET_HEADER_

#include <RpcSession.h>

#include "Context.h"

#include <QWidget>
#include <QString>
#include <QBoxLayout>

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
	void onRefresh();

private:
	void clear();

private:
	ContextPtr context_;
	QString name_;

	QBoxLayout* layout_;

	bool firstShow_;
};

#endif // PAGEENGINEWIDGET_HEADER_

