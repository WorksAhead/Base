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
class ContentWidget;

class Page : public QWidget {
private:
	Q_OBJECT

private:
	typedef QPair<QString, int> ImageIndex;

public:
	Page(ContextPtr context, const QString& name, QWidget* parent = 0);
	~Page();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onScroll(int);
	void onBack();
	void onRefresh();
	void onTick();

private:
	void showMore(int);
	void submit();
	void clear();

	QString makeImageFilename(const ImageIndex&);
	void cancelAllImageLoadTasks();
	void loadImage(const ImageIndex&, bool highPriority = false);
	void setImage(const ImageIndex&);

private:
	ContextPtr context_;
	QString name_;
	FlowLayout* flowLayout_;

	ContentWidget* content_;

	QTimer* timer_;

	Rpc::ContentBrowserPrx browser_;

	QMap<QString, PageItem*> pageItems_;

	QSet<ImageIndex> loadedImages_;
	QMap<ImageIndex, ASyncDownloadTask*> imageLoadTasks_;
	QList<ImageIndex> pendingImages_;

	Ui::Page ui_;
	bool firstShow_;
};

#endif // PAGE_HEADER_

