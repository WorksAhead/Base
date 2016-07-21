#include "ContentWidget.h"
#include "ContentDownloadDialog.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncDownloadContentTask.h"

#include <QPainter>
#include <QClipboard>
#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ContentWidget::ContentWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	thumbnailLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	thumbnailLayout_->setMargin(2);
	thumbnailLayout_->setSpacing(5);

	thumbnailWidget_ = new QWidget;
	thumbnailWidget_->setLayout(thumbnailLayout_);
	ui_.thumbnailScrollArea->setWidget(thumbnailWidget_);

	QObject::connect(ui_.downloadButton, &QPushButton::clicked, this, &ContentWidget::onDownload);
	QObject::connect(ui_.installEngineButton, &QPushButton::clicked, this, &ContentWidget::onInstallEngine);
	QObject::connect(ui_.copySummaryButton, &QPushButton::clicked, this, &ContentWidget::onCopySummary);
	QObject::connect(ui_.copyIdButton, &QPushButton::clicked, this, &ContentWidget::onCopyId);
}

ContentWidget::~ContentWidget()
{
}

void ContentWidget::setId(const QString& id)
{
	id_ = id;
}

const QString& ContentWidget::id() const
{
	return id_;
}

void ContentWidget::setTitle(const QString& text)
{
	ui_.titleLabel->setText(text);
}

void ContentWidget::setSummary(const QString& text)
{
	ui_.summaryLabel->setText(text);
}

void ContentWidget::setDescription(const QString& text)
{
	ui_.descriptionLabel->setText(text);
}

void ContentWidget::setEngineVersion(int index, const QString& name, const QString& version)
{
	ui_.engineVersionsBox->insertItem(index, name + " " + version);
	engineVersions_[index].first = name;
	engineVersions_[index].second = version;
}

void ContentWidget::setEngineVersionCount(int count)
{
	ui_.engineVersionsBox->clear();
	engineVersions_.clear();
	engineVersions_.resize(count);
}

void ContentWidget::setImage(int index, const QPixmap& pixmap)
{
	QLayoutItem* li = thumbnailLayout_->itemAt(index);
	if (li && li->widget()) {
		QLabel* label = qobject_cast<QLabel*>(li->widget());
		if (label) {
			label->setPixmap(pixmap.scaled(192, 108, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
	}

	screenshots_[index] = pixmap;

	if (index == 0)
	{
		ui_.screenshotViewer->setPixmap(pixmap);
	}
}

void ContentWidget::setImageCount(int count)
{
	while (thumbnailLayout_->count()) {
		QLayoutItem* li = thumbnailLayout_->takeAt(0);
		li->widget()->deleteLater();
		delete li;
	}

	if (count > 0)
	{
		for (int i = 0; i < count; ++i) {
			QLabel* label = new QLabel;
			label->setFixedSize(192, 108);
			label->setText("Loading");
			label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			thumbnailLayout_->addWidget(label);
		}

		thumbnailLayout_->addStretch(1);
	}

	if (count == 0) {
		ui_.screenshotViewer->setVisible(false);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else if (count == 1) {
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else {
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(true);
	}

	ui_.screenshotViewer->setPixmap(QPixmap());

	screenshots_.clear();
	screenshots_.resize(count);
}

void ContentWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QPoint pos = thumbnailWidget_->mapFrom(this, e->pos());
		if (thumbnailWidget_->rect().contains(pos))
		{
			QWidget* w = thumbnailWidget_->childAt(pos);
			const int index = thumbnailLayout_->indexOf(w);
			if (index >= 0 && index < screenshots_.count())
			{
				ui_.screenshotViewer->setPixmap(screenshots_.at(index));
			}
		}
	}
}

void ContentWidget::resizeEvent(QResizeEvent* e)
{
	ui_.screenshotViewer->setFixedHeight(ui_.screenshotViewer->width() / 1.7777777777);
}

void ContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContentWidget::onDownload()
{
	QString engineName = engineVersions_[0].first;
	QString engineVersion = engineVersions_[0].second;

	bool installEngine = false;

	const int engineState = context_->getEngineState(engineName.toStdString(), engineVersion.toStdString());

	if (engineState == EngineState::not_installed)
	{
		ContentDownloadDialog d;
		d.setEngineVersionAboutToBeDownloaded(engineName, engineVersion);
		int ret = d.exec();
		if (ret != 1) {
			return;
		}

		installEngine = d.isInstallEngineChecked();
	}

	if (installEngine)
	{
	}

	int state = ContentState::not_downloaded;

	if (!context_->changeContentState(id_.toStdString(), state, ContentState::downloading)) {
		if (state == ContentState::downloading) {
			QMessageBox::information(this, "Base", "This content is now downloading");
		}
		else if (state == ContentState::downloaded) {
			QMessageBox::information(this, "Base", "This content is already downloaded");
		}
		else if (state == ContentState::removing) {
			QMessageBox::information(this, "Base", "This content is now removing");
		}
		return;
	}

	Rpc::DownloaderPrx downloader;
	Rpc::ErrorCode ec = context_->session->downloadContent(id_.toStdString(), downloader);
	if (ec != Rpc::ec_success) {
		state = ContentState::downloading;
		context_->changeContentState(id_.toStdString(), state, ContentState::not_downloaded);
		QMessageBox::information(this, "Base", "Unable to download this content");
		return;
	}

	fs::path fullPath(context_->contentPath(id_.toStdString()));
	fullPath /= "content";

	std::unique_ptr<ASyncDownloadContentTask> task(new ASyncDownloadContentTask(context_, downloader));
	task->setInfoHead(QString("Download %1").arg(ui_.titleLabel->text()).toStdString());
	task->setContentId(id_.toStdString());
	task->setFilename(fullPath.string());
	context_->addTask(task.release());
}

void ContentWidget::onInstallEngine()
{
	const int index = ui_.engineVersionsBox->currentIndex();
	QString engineName = engineVersions_[index].first;
	QString engineVersion = engineVersions_[index].second;

	int state = EngineState::not_installed;

	if (!context_->changeEngineState(engineName.toStdString(), engineVersion.toStdString(), state, EngineState::installing)) {
		if (state == EngineState::installing) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is now installing").arg(engineName).arg(engineVersion));
		}
		else if (state == EngineState::installed) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is already installed").arg(engineName).arg(engineVersion));
		}
		else if (state == EngineState::removing) {
			QMessageBox::information(this, "Base",
				QString("%1 %2 is now removing").arg(engineName).arg(engineVersion));
		}
		return;
	}

	Rpc::DownloaderPrx downloader;
	Rpc::ErrorCode ec = context_->session->downloadEngineVersion(engineName.toStdString(), engineVersion.toStdString(), downloader);
	if (ec != Rpc::ec_success) {
		state = EngineState::installing;
		context_->changeEngineState(engineName.toStdString(), engineVersion.toStdString(), state, EngineState::not_installed);
		QMessageBox::information(this, "Base",
			QString("Unable to download %1 %2").arg(engineName).arg(engineVersion));
		return;
	}

	std::unique_ptr<ASyncInstallEngineTask> task(new ASyncInstallEngineTask(context_, downloader));
	task->setInfoHead(QString("Install %1 %2").arg(engineName).arg(engineVersion).toStdString());
	task->setEngineVersion(engineName.toStdString(), engineVersion.toStdString());
	task->setPath(context_->enginePath(engineName.toStdString(), engineVersion.toStdString()));
	context_->addTask(task.release());
}

void ContentWidget::onCopyId()
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(id_);
}

void ContentWidget::onCopySummary()
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(ui_.titleLabel->text() + "\n" + ui_.summaryLabel->text());
}

