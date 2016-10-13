#include "PageContentContentWidget.h"
#include "ContentDownloadDialog.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncDownloadContentTask.h"
#include "ContentImageLoader.h"

#include <QPainter>
#include <QMouseEvent>

#include <QScrollBar>
#include <QLabel>
#include <QGridLayout>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <sstream>
#include <string>

namespace fs = boost::filesystem;

PageContentContentWidget::PageContentContentWidget(ContextPtr context, const QString& contentId, QWidget* parent)
	: context_(context), contentId_(contentId), QWidget(parent)
{
	QWidget* w = new QWidget;
	ui_.setupUi(w);

	ui_.screenshotViewer->setAspectRatio(16.0 / 9.0);

	thumbnailLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	thumbnailLayout_->setMargin(2);
	thumbnailLayout_->setSpacing(5);

	thumbnailWidget_ = new QWidget;
	thumbnailWidget_->setLayout(thumbnailLayout_);
	ui_.thumbnailScrollArea->setWidget(thumbnailWidget_);

	scrollArea_ = new QScrollArea;
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea_->setWidgetResizable(true);
	scrollArea_->setWidget(w);

	QLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(scrollArea_);
	setLayout(layout);

	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &PageContentContentWidget::onImageLoaded);
	QObject::connect(ui_.downloadButton, &QPushButton::clicked, this, &PageContentContentWidget::onDownload);

	firstShow_ = true;
}

PageContentContentWidget::~PageContentContentWidget()
{
}

void PageContentContentWidget::refresh()
{
	scrollArea_->verticalScrollBar()->setValue(0);

	Rpc::ContentInfo ci;
	Rpc::ErrorCode ec = context_->session->getContentInfo(contentId_.toStdString(), ci);
	if (ec != Rpc::ec_success) {
		ui_.titleLabel->setText("Content not found");
		ui_.summaryLabel->setVisible(false);
		ui_.screenshotWidget->setVisible(false);
		ui_.downloadButton->setVisible(false);
		ui_.descriptionLabel->setVisible(false);
		return;
	}

	std::ostringstream summary;
	summary << ci.user << " " << ci.upTime << "\n";
	summary << "\nSupported Engine Versions:\n";
	summary << ci.engineName << " " << ci.engineVersion << "\n";
	summary << "\nID:\n" << ci.id << "\n";
	if (!ci.parentId.empty()) {
		summary << "\nParent Id:\n" << ci.parentId << "\n";
	}

	std::vector<std::string> versions;
	boost::split(versions, ci.engineVersion, boost::is_any_of("|"));

	ui_.titleLabel->setText(ci.title.c_str());
	ui_.summaryLabel->setText(summary.str().c_str());
	ui_.descriptionLabel->setText(ci.desc.c_str());

	if (versions.size()) {
		supportedEngineVersion_.first = ci.engineName.c_str();
		supportedEngineVersion_.second = versions[0].c_str();
	}
	
	setImageCount(ci.imageCount - 1);

	for (int i = 1; i < ci.imageCount; ++i) {
		context_->contentImageLoader->load(ci.id.c_str(), i, true);
	}
}

const QString& PageContentContentWidget::contentId() const
{
	return contentId_;
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

void PageContentContentWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		refresh();
		firstShow_ = false;
	}
}

void PageContentContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentContentWidget::onImageLoaded(const QString& id, int index, const QPixmap& image)
{
	if (id == contentId_ && index > 0) {
		setImage(index - 1, image);
	}
}

void PageContentContentWidget::onDownload()
{
	int state = ContentState::not_downloaded;

	if (!context_->changeContentState(contentId_.toStdString(), state, ContentState::downloading))
	{
		if (state == ContentState::downloading) {
			context_->prompt(0, "This content is now downloading");
		}
		else if (state == ContentState::downloaded) {
			context_->prompt(0, "This content is already downloaded");
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
		context_->prompt(0, "Unable to download this content");
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
			label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
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

