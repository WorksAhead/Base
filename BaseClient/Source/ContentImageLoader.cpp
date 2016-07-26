#include "ContentImageLoader.h"
#include "ASyncDownloadTask.h"

#include <QTime>

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
	while (it.hasNext()) {
		it.next();
		ASyncDownloadTask* task = it.value();
		task->cancel();
		delete task;
	}

	imageLoadTasks_.clear();
	pendingImages_.clear();
}

void ContentImageLoader::load(const QString& id, int index, bool highPriority)
{
	ImageIndex imageIndex(id, index);

	if (loadedImages_.contains(imageIndex)) {
		setImage(imageIndex);
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

		if (loadedImages_.contains(imageIndex)) {
			setImage(imageIndex);
			pendingImages_.removeAt(index);
			continue;
		}

		ASyncDownloadTask* task = imageLoadTasks_.value(imageIndex, 0);

		if (task)
		{
			const int state = task->state();

			if (state == ASyncTask::state_finished) {
				setImage(imageIndex);
				loadedImages_.insert(imageIndex);
			}

			if (state == ASyncTask::state_finished || state == ASyncTask::state_failed) {
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

			task = new ASyncDownloadTask(context_, downloader);
			task->setFilename(makeImageFilename(imageIndex).toStdString());
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
		path /= imageIndex.first.toStdString() + "_cover.jpg";
	}
	else {
		path /= imageIndex.first.toStdString() + "_image_" + std::to_string(imageIndex.second) + ".jpg";
	}
	return QString(path.string().c_str());
}

void ContentImageLoader::setImage(const ImageIndex& imageIndex)
{
	const QPixmap& image = QPixmap(makeImageFilename(imageIndex), "JPG");
	Q_EMIT loaded(imageIndex.first, imageIndex.second, image);
}

