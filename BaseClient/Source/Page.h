#ifndef PAGE_HEADER_
#define PAGE_HEADER_

#include "ui_Page.h"

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
class PageItem;

class Page : public QWidget {
private:
	Q_OBJECT

public:
	Page(ContextPtr context, const QString& name, QWidget* parent = 0);
	~Page();

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onSliderMoved(int);
	void onRefresh();
	void onTick();

private:
	void showMore(int);
	void submit();
	void clear();
	void loadCover(const QString& id);
	QString makeCoverFilename(const QString& id);

private:
	ContextPtr context_;
	QString name_;
	FlowLayout* flowLayout_;

	QTimer* timer_;

	Rpc::ContentBrowserPrx browser_;
	QMap<QString, PageItem*> pageItems_;
	QMap<QString, ASyncDownloadTask*> coverDownloadTasks_;
	QList<QString> pendingCovers_;
	QSet<QString> loadedCovers_;

	Ui::Page ui_;
	bool firstShow_;
};

#endif // PAGE_HEADER_

