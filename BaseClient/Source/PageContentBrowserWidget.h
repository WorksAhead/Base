#ifndef PAGECONTENTBROWSERWIDGET_HEADER_
#define PAGECONTENTBROWSERWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include <RpcSession.h>

#include <QWidget>
#include <QScrollArea>
#include <QPixmap>
#include <QString>
#include <QMap>

// forward declaration
class PageContentItemWidget;

class PageContentBrowserWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageContentBrowserWidget(ContextPtr context, const QString& name, const QString& category, QWidget* parent = 0);
	~PageContentBrowserWidget();

public:
	void refresh();
	void setCoverSize(int);

Q_SIGNALS:
	void contentClicked(const QString& id);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onScroll(int);
	void onImageLoaded(const QString& id, int index, const QPixmap&);

private:
	void clear();
	void showMore(int);

private:
	ContextPtr context_;
	QString name_;
	QString category_;
	QScrollArea* scrollArea_;
	FlowLayout* contentsLayout_;

	Rpc::ContentBrowserPrx browser_;

	QMap<QString, PageContentItemWidget*> items_;

	bool firstShow_;
	int coverSize_;
};

#endif // PAGECONTENTBROWSERWIDGET_HEADER_

