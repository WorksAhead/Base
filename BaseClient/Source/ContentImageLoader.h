#ifndef CONTENTIMAGELOADER_HEADER_
#define CONTENTIMAGELOADER_HEADER_

#include <RpcSession.h>

#include "Context.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QPixmap>
#include <QTimer>

// forward declaration
class ASyncDownloadTask;

class ContentImageLoader : public QObject {
private:
	Q_OBJECT

private:
	typedef QPair<QString, int> ImageIndex;

public:
	ContentImageLoader(ContextPtr context, QObject* parent = 0);
	~ContentImageLoader();

	void load(const QString& id, int index, bool highPriority = false);

Q_SIGNALS:
	void loaded(const QString& id, int index, const QPixmap&);

private Q_SLOTS:
	void onTick();

private:
	QString makeImageFilename(const ImageIndex&);
	void setImage(const ImageIndex& imageIndex);

private:
	ContextPtr context_;

	QTimer* timer_;

	QSet<ImageIndex> loadedImages_;
	QMap<ImageIndex, ASyncDownloadTask*> imageLoadTasks_;
	QList<ImageIndex> pendingImages_;
};

#endif // CONTENTIMAGELOADER_HEADER_

