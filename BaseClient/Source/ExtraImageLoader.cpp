#include "ExtraImageLoader.h"
#include "ASyncDownloadTask.h"
#include "QtUtils.h"

#include <QTime>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ExtraImageLoader::ExtraImageLoader(ContextPtr context, QObject* parent) : context_(context), QObject(parent)
{
	timer_ = new QTimer(this);
	timer_->start(50);

	QObject::connect(timer_, &QTimer::timeout, this, &ExtraImageLoader::onTick);
}

ExtraImageLoader::~ExtraImageLoader()
{
	QMapIterator<QString, ASyncDownloadTask*> it(imageLoadTasks_);
	while (it.hasNext()) {
		it.next();
		ASyncDownloadTask* task = it.value();
		task->cancel();
		delete task;
	}

	imageLoadTasks_.clear();
	pendingImages_.clear();
}

void ExtraImageLoader::load(const QString& id, bool highPriority)
{
	if (loadedImages_.contains(id)) {
		setImage(id);
		return;
	}

	if (highPriority) {
		pendingImages_.push_front(id);
	}
	else {
		pendingImages_.append(id);
	}
}

void ExtraImageLoader::onTick()
{
	const int maxTasks = 5;

	QTime t;
	t.start();

	int index = 0;

	while (index < qMin(maxTasks, pendingImages_.count()))
	{
		const QString& id = pendingImages_.at(index);

		if (loadedImages_.contains(id)) {
			setImage(id);
			pendingImages_.removeAt(index);
			continue;
		}

		ASyncDownloadTask* task = imageLoadTasks_.value(id, 0);

		if (task)
		{
			const int state = task->state();

			if (state == ASyncTask::state_finished) {
				setImage(id);
				loadedImages_.insert(id);
			}

			if (state == ASyncTask::state_finished || state == ASyncTask::state_failed) {
				delete task;
				imageLoadTasks_.remove(id);
				pendingImages_.removeAt(index);
				continue;
			}
		}
		else
		{
			Rpc::DownloaderPrx downloader;

			Rpc::ErrorCode ec = context_->session->downloadExtraImage(id.toStdString(), downloader);
			if (ec != Rpc::ec_success) {
				pendingImages_.removeAt(index);
				continue;
			}

			task = new ASyncDownloadTask(downloader);
			task->setFilename(toLocal8bit(makeImageFilename(id)));
			task->start();
			imageLoadTasks_.insert(id, task);
		}

		if (t.elapsed() > 10) {
			break;
		}

		++index;
	}
}

QString ExtraImageLoader::makeImageFilename(const QString& id)
{
	fs::path path = context_->cachePath();
	path /= id.toStdString() + "_image_0.jpg";
	return QString::fromLocal8Bit(path.string().c_str());
}

void ExtraImageLoader::setImage(const QString& id)
{
	const QPixmap& image = QPixmap(makeImageFilename(id), "JPG");
	Q_EMIT loaded(id, image);
}

