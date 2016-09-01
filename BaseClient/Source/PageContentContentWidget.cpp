#include "PageContentContentWidget.h"
#include "ContentDownloadDialog.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncDownloadContentTask.h"

#include <QPainter>
#include <QClipboard>
#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

PageContentContentWidget::PageContentContentWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	ui_.screenshotViewer->setAspectRatio(16.0 / 9.0);

	thumbnailLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	thumbnailLayout_->setMargin(2);
	thumbnailLayout_->setSpacing(5);

	thumbnailWidget_ = new QWidget;
	thumbnailWidget_->setLayout(thumbnailLayout_);
	ui_.thumbnailScrollArea->setWidget(thumbnailWidget_);

	QObject::connect(ui_.downloadButton, &QPushButton::clicked, this, &PageContentContentWidget::onDownload);
}

PageContentContentWidget::~PageContentContentWidget()
{
}

void PageContentContentWidget::setContentId(const QString& id)
{
	const int state = context_->getContentState(id.toStdString());
	//if (state == ContentState::not_downloaded) {
	//	ui_.downloadButton->setEnabled(true);
	//}
	//else {
	//	ui_.downloadButton->setEnabled(false);
	//}
	contentId_ = id;
}

const QString& PageContentContentWidget::contentId() const
{
	return contentId_;
}

void PageContentContentWidget::setTitle(const QString& text)
{
	ui_.titleLabel->setText(text);
}

void PageContentContentWidget::setSummary(const QString& text)
{
	ui_.summaryLabel->setText(text);
}

void PageContentContentWidget::setDescription(const QString& text)
{
	ui_.descriptionLabel->setText(text);
}

void PageContentContentWidget::setSupportedEngineVersion(const QString& name, const QString& version)
{
	supportedEngineVersion_.first = name;
	supportedEngineVersion_.second = version;
}

void PageContentContentWidget::setImage(int index, const QPixmap& pixmap)
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

void PageContentContentWidget::setImageCount(int count)
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
		ui_.screenshotWidget->setVisible(false);
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(true);
	}
	else if (count == 1) {
		ui_.screenshotWidget->setVisible(true);
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else {
		ui_.screenshotWidget->setVisible(true);
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(true);
	}

	ui_.screenshotViewer->setPixmap(QPixmap());

	screenshots_.clear();
	screenshots_.resize(count);
}

void PageContentContentWidget::mousePressEvent(QMouseEvent* e)
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

void PageContentContentWidget::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);
}

void PageContentContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentContentWidget::onDownload()
{
	int state = ContentState::not_downloaded;

	if (!context_->changeContentState(contentId_.toStdString(), state, ContentState::downloading))
	{
		if (state == ContentState::downloading) {
			QMessageBox::information(this, "Base", "This content is now downloading");
		}
		else if (state == ContentState::downloaded) {
			QMessageBox::information(this, "Base", "This content is already downloaded");
		}
		return;
	}

	QString engineName = supportedEngineVersion_.first;
	QString engineVersion = supportedEngineVersion_.second;

	bool installEngine = false;

	int engineState = context_->getEngineState(EngineVersion(engineName.toStdString(), engineVersion.toStdString()));

	if (engineState == EngineState::not_installed)
	{
		ContentDownloadDialog d;
		d.setEngineVersionAboutToBeDownloaded(engineName, engineVersion);
		int ret = d.exec();
		if (ret != 1) {
			state = ContentState::downloading;
			context_->changeContentState(contentId_.toStdString(), state, ContentState::not_downloaded);
			return;
		}

		installEngine = d.isInstallEngineChecked();
	}

	Rpc::DownloaderPrx downloader;
	Rpc::ErrorCode ec = context_->session->downloadContent(contentId_.toStdString(), downloader);
	if (ec != Rpc::ec_success) {
		state = ContentState::downloading;
		context_->changeContentState(contentId_.toStdString(), state, ContentState::not_downloaded);
		QMessageBox::information(this, "Base", "Unable to download this content");
		return;
	}

	boost::shared_ptr<ASyncDownloadContentTask> task(new ASyncDownloadContentTask(context_, downloader));
	task->setInfoHead(QString("Download %1").arg(ui_.titleLabel->text()).toLocal8Bit().data());
	task->setContentId(contentId_.toStdString());
	task->setFilename((fs::path(context_->contentPath(contentId_.toStdString())) / "content").string());
	context_->addTask(task);

	if (installEngine) {
		context_->installEngine(EngineVersion(engineName.toStdString(), engineVersion.toStdString()));
	}
}
