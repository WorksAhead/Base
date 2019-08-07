#include "ContentImageLoader.h"
#include "ASyncDownloadTask.h"
#include "QtUtils.h"

#include <QTime>
#include <QImageReader>
#include <QMovie>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ContentImageLoader::ContentImageLoader(ContextPtr context, QObject* parent) : context_(context), QObject(parent)
{
	timer_ = new QTimer(this);
	timer_->start(50);

	QObject::connect(timer_, &QTimer::timeout, this, &ContentImageLoader::onTick);
}

ContentImageLoader::~ContentImageLoader()
{
	QMapIterator<ImageIndex, ASyncDownloadTask*> it(imageLoadTasks_);

	while (it.hasNext())
	{
		it.next();
		ASyncDownloadTask* task = it.value();
		task->cancel();
		delete task;
	}

	imageLoadTasks_.clear();
	pendingImages_.clear();

	QMapIterator<ImageIndex, QPixmap*> it2(loadedImages_);

	while (it2.hasNext())
	{
		it2.next();
		delete it2.value();
	}

	QMapIterator<ImageIndex, QMovie*> it3(loadedAnimations_);

	while (it3.hasNext())
	{
		it3.next();
		delete it3.value();
	}
}

void ContentImageLoader::load(const QString& id, int index, bool highPriority)
{
	ImageIndex imageIndex(id, index);

	if (loadedAnimations_.contains(imageIndex))
	{
		Q_EMIT animationLoaded(imageIndex.first, imageIndex.second, loadedAnimations_[imageIndex]);
		return;
	}
	else if (loadedImages_.contains(imageIndex))
	{
		Q_EMIT imageLoaded(imageIndex.first, imageIndex.second, loadedImages_[imageIndex]);
		return;
	}

	if (loadFromLocalFile(imageIndex)) {
		return;
	}

	if (highPriority) {
		pendingImages_.push_front(imageIndex);
	}
	else {
		pendingImages_.append(imageIndex);
	}
}

void ContentImageLoader::onTick()
{
	const int maxTasks = 5;

	QTime t;
	t.start();

	int index = 0;

	while (index < qMin(maxTasks, pendingImages_.count()))
	{
		const ImageIndex& imageIndex = pendingImages_.at(index);

		if (loadedAnimations_.contains(imageIndex))
		{
			Q_EMIT animationLoaded(imageIndex.first, imageIndex.second, loadedAnimations_[imageIndex]);

			pendingImages_.removeAt(index);
			continue;
		}
		else if (loadedImages_.contains(imageIndex))
		{
			Q_EMIT imageLoaded(imageIndex.first, imageIndex.second, loadedImages_[imageIndex]);

			pendingImages_.removeAt(index);
			continue;
		}

		ASyncDownloadTask* task = imageLoadTasks_.value(imageIndex, 0);

		if (task)
		{
			const int state = task->state();

			if (state == ASyncTask::state_finished)
			{
				loadFromLocalFile(imageIndex);
			}

			if (state == ASyncTask::state_finished || state == ASyncTask::state_failed)
			{
				delete task;
				imageLoadTasks_.remove(imageIndex);
				pendingImages_.removeAt(index);
				continue;
			}
		}
		else
		{
			Rpc::DownloaderPrx downloader;

			Rpc::ErrorCode ec = context_->session->downloadContentImage(imageIndex.first.toStdString(), imageIndex.second, downloader);

			if (ec != Rpc::ec_success) {
				pendingImages_.removeAt(index);
				continue;
			}

			task = new ASyncDownloadTask(downloader);
			task->setFilename(toLocal8bit(makeImageFilename(imageIndex)));
			task->start();
			imageLoadTasks_.insert(imageIndex, task);
		}

		if (t.elapsed() > 10) {
			break;
		}

		++index;
	}
}

QString ContentImageLoader::makeImageFilename(const ImageIndex& imageIndex)
{
	fs::path path = context_->cachePath();

	if (imageIndex.second == 0) {
		path /= imageIndex.first.toStdString() + "_cover";
	}
	else {
		path /= imageIndex.first.toStdString() + "_image_" + std::to_string(imageIndex.second);
	}

	return QString::fromLocal8Bit(path.string().c_str());
}

bool ContentImageLoader::loadFromLocalFile(const ImageIndex& imageIndex)
{
	QString path = makeImageFilename(imageIndex);

	QImageReader reader(path);

	QImage image = reader.read();

	if (image.isNull()) {
		return false;
	}

	if (reader.supportsAnimation())
	{
		QMovie* movie = new QMovie(path);

		movie->start();

		loadedAnimations_.insert(imageIndex, movie);

		Q_EMIT animationLoaded(imageIndex.first, imageIndex.second, movie);
	}
	else
	{
		QPixmap* pixmap = new QPixmap(QPixmap::fromImage(image));

		loadedImages_.insert(imageIndex, pixmap);

		Q_EMIT imageLoaded(imageIndex.first, imageIndex.second, pixmap);
	}

	return true;
}

