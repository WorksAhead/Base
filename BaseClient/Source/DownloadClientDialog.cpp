#include "DownloadClientDialog.h"
#include "ASyncDownloadClientTask.h"
#include "ASyncTaskWidget.h"
#include "QtUtils.h"

#include <ErrorMessage.h>

#include <QCoreApplication>
#include <QProcess>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

DownloadClientDialog::DownloadClientDialog(Rpc::StartPrx startPrx, QWidget* parent) : QDialog(parent), startPrx_(startPrx)
{
	setWindowFlags(Qt::WindowTitleHint);

	ui_.setupUi(this);

	refreshTimer_ = new QTimer(this);

	refreshTimer_->start(100);

	if (!beginDownload()) {
		return;
	}

	QObject::connect(refreshTimer_, &QTimer::timeout, this, &DownloadClientDialog::onRefresh);
}

DownloadClientDialog::~DownloadClientDialog()
{
}

void DownloadClientDialog::onRefresh()
{
	taskWidget_->refresh();

	switch (taskWidget_->task()->state())
	{
	case ASyncTask::state_idle:
	case ASyncTask::state_running:
	case ASyncTask::state_cancelled:
		break;

	case ASyncTask::state_failed:
		{
			int ret = QMessageBox::warning(0, "Base", tr("Failed to download Client."), QMessageBox::Retry|QMessageBox::Default, QMessageBox::Abort);
			if (ret == QMessageBox::Retry) {
				ui_.taskStackedWidget->removeWidget(taskWidget_);
				if (!beginDownload()) {
					refreshTimer_->stop();
					done(0);
				}
			}
			else {
				refreshTimer_->stop();
				done(0);
			}
		}
		break;

	case ASyncTask::state_finished:
		{
			ASyncDownloadClientTask* task = (ASyncDownloadClientTask*)taskWidget_->task().get();

			fs::path dir = fs::canonical(task->path());
			fs::path workDir = dir / "Bin";
			fs::path program = workDir / "BaseClient.exe";

			QStringList arguments;

			arguments << "update";
			arguments << dir.string().c_str();
			arguments << fs::canonical(toLocal8bit(QCoreApplication::applicationDirPath())).parent_path().string().c_str();

			if (!QProcess::startDetached(QString::fromLocal8Bit(program.string().c_str()),
				arguments, QString::fromLocal8Bit(workDir.string().c_str()))) {
				QMessageBox::warning(0, "Base", tr("Failed to start Client update."));
			}

			done(0);
		}
		break;
	}
}

bool DownloadClientDialog::beginDownload()
{
	Rpc::ErrorCode ec = startPrx_->downloadClient(downloaderPrx_);
	if (ec != Rpc::ErrorCode::ec_success) {
		QMessageBox::critical(0, "Base", errorMessage(ec));
		return false;
	}

	boost::shared_ptr<ASyncDownloadClientTask> task(new ASyncDownloadClientTask(downloaderPrx_));
	task->setInfoHead("Update");
	task->start();

	taskWidget_ = new ASyncTaskWidget(task);
	taskWidget_->refresh();

	ui_.taskStackedWidget->addWidget(taskWidget_);

	return true;
}

