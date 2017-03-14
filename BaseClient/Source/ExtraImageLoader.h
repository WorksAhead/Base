#ifndef EXTRAIMAGELOADER_HEADER_
#define EXTRAIMAGELOADER_HEADER_

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

class ExtraImageLoader : public QObject {
private:
	Q_OBJECT

public:
	ExtraImageLoader(ContextPtr context, QObject* parent = 0);
	~ExtraImageLoader();

	void load(const QString& id, bool highPriority = false);

Q_SIGNALS:
	void loaded(const QString& id, const QPixmap&);

private Q_SLOTS:
	void onTick();

private:
	QString makeImageFilename(const QString&);
	void setImage(const QString&);

private:
	ContextPtr context_;

	QTimer* timer_;

	QSet<QString> loadedImages_;
	QMap<QString, ASyncDownloadTask*> imageLoadTasks_;
	QList<QString> pendingImages_;
};


#endif // EXTRAIMAGELOADER_HEADER_

