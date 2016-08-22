#ifndef PAGECONTENTWIDGET_HEADER_
#define PAGECONTENTWIDGET_HEADER_

#include "ui_PageContentWidget.h"

#include <RpcSession.h>

#include "Context.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTimer>

// forward declaration
class ASyncDownloadTask;
class FlowLayout;
class PageContentItemWidget;
class ContentWidget;

class PageContentWidget : public QWidget {
private:
	Q_OBJECT

private:
	typedef QPair<QString, int> ImageIndex;

public:
	PageContentWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageContentWidget();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onScroll(int);
	void onBack();
	void onRefresh();
	void onImageLoaded(const QString& id, int index, const QPixmap&);

private:
	void showMore(int);
	void submit();
	void clear();

private:
	ContextPtr context_;
	QString name_;
	FlowLayout* flowLayout_;

	ContentWidget* content_;

	Rpc::ContentBrowserPrx browser_;

	QMap<QString, PageContentItemWidget*> pageItems_;

	Ui::PageContentWidget ui_;
	bool firstShow_;
};

#endif // PAGECONTENTWIDGET_HEADER_

