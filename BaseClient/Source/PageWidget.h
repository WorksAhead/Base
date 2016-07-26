#ifndef PAGEWIDGET_HEADER_
#define PAGEWIDGET_HEADER_

#include "ui_PageWidget.h"

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
class PageItemWidget;
class ContentWidget;

class PageWidget : public QWidget {
private:
	Q_OBJECT

private:
	typedef QPair<QString, int> ImageIndex;

public:
	PageWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageWidget();

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

	QMap<QString, PageItemWidget*> pageItems_;

	Ui::PageWidget ui_;
	bool firstShow_;
};

#endif // PAGEWIDGET_HEADER_

