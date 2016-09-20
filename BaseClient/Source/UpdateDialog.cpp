#include "UpdateDialog.h"
#include "ASyncUpdateClientTask.h"
#include "ASyncTaskWidget.h"
#include "QtUtils.h"

#include <ErrorMessage.h>

#include <QCoreApplication>
#include <QProcess>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

UpdateDialog::UpdateDialog(const QString& sourceDir, const QString& targetDir, QWidget* parent)
	: sourceDir_(sourceDir), targetDir_(targetDir), QDialog(parent)
{
	setWindowFlags(Qt::WindowTitleHint);

	ui_.setupUi(this);

	beginTimer_ = new QTimer(this);

	beginTimer_->start(1000);

	refreshTimer_ = new QTimer(this);

	QObject::connect(beginTimer_, &QTimer::timeout, this, &UpdateDialog::onBegin);
	QObject::connect(refreshTimer_, &QTimer::timeout, this, &UpdateDialog::onRefresh);
}

UpdateDialog::~UpdateDialog()
{
}

void UpdateDialog::onBegin()
{
	beginUpdate();

	beginTimer_->stop();
	refreshTimer_->start(100);
}

void UpdateDialog::onRefresh()
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
			int ret = QMessageBox::warning(0, "Base", tr("Failed to update Client."), QMessageBox::Retry|QMessageBox::Default, QMessageBox::Abort);
			if (ret == QMessageBox::Retry) {
				ui_.taskStackedWidget->removeWidget(taskWidget_);
				beginUpdate();
			}
			else {
				QMessageBox::information(0, "Base", tr("Client may be left in invalid state. Please contact administrator for help."));
				refreshTimer_->stop();
				done(0);
			}
		}
		break;

	case ASyncTask::state_finished:
		{
			fs::path dir = fs::canonical(toLocal8bit(targetDir_));
			fs::path workDir = dir / "Bin";
			fs::path program = workDir / "BaseClient.exe";

			if (!QProcess::startDetached(QString::fromLocal8Bit(program.string().c_str()),
				QStringList(), QString::fromLocal8Bit(workDir.string().c_str()))) {
				QMessageBox::warning(0, "Base", tr("Failed to launch Client."));
			}

			done(0);
		}
		break;
	}
}

void UpdateDialog::beginUpdate()
{
	boost::shared_ptr<ASyncUpdateClientTask> task(new ASyncUpdateClientTask());
	task->setInfoHead("Update");
	task->setSourcePath(toLocal8bit(sourceDir_));
	task->setTargetPath(toLocal8bit(targetDir_));
	task->start();

	taskWidget_ = new ASyncTaskWidget(task);
	taskWidget_->refresh();

	ui_.taskStackedWidget->addWidget(taskWidget_);
}

