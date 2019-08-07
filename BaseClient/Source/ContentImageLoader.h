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
	void imageLoaded(const QString& id, int index, QPixmap*);
	void animationLoaded(const QString& id, int index, QMovie*);

private Q_SLOTS:
	void onTick();

private:
	QString makeImageFilename(const ImageIndex&);
	bool loadFromLocalFile(const ImageIndex&);

private:
	ContextPtr context_;

	QTimer* timer_;

	QMap<ImageIndex, QPixmap*> loadedImages_;
	QMap<ImageIndex, QMovie*> loadedAnimations_;

	QMap<ImageIndex, ASyncDownloadTask*> imageLoadTasks_;

	QList<ImageIndex> pendingImages_;
};

#endif // CONTENTIMAGELOADER_HEADER_

