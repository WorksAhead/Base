#include "PageContentContentWidget.h"
#include "ContentDownloadDialog.h"
#include "ASyncInstallEngineTask.h"
#include "ASyncDownloadContentTask.h"
#include "ContentImageLoader.h"
#include "VideoPlayerWidget.h"
#include "ImageViewerWidget.h"
#include "Emoji.h"
#include "URLUtils.h"

#include <QPainter>
#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>
#include <QGridLayout>
#include <QMenu>
#include <QClipboard>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <sstream>
#include <string>
#include <limits>

namespace fs = boost::filesystem;

PageContentContentWidget::PageContentContentWidget(ContextPtr context, const QString& contentId, QWidget* parent)
	: context_(context), contentId_(contentId), QWidget(parent)
{
	QWidget* w = new QWidget;
	ui_.setupUi(w);

	ui_.stackedWidget->setAspectRatio(16.0 / 9.0);

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

	QObject::connect(ui_.versionInfoLabel, &QLabel::linkActivated, [this](const QString& link)
	{
		if (link == "All") {
			refreshRelatedVersions(std::numeric_limits<int>::max());
		}
		else {
			context_->openUrl(link.toStdString());
		}
	});

	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &PageContentContentWidget::onImageLoaded);
	QObject::connect(ui_.downloadButton, &QPushButton::clicked, this, &PageContentContentWidget::onDownload);
	QObject::connect(ui_.summary, &QTextBrowser::anchorClicked, this, &PageContentContentWidget::onAnchorClicked);
	QObject::connect(ui_.description, &QTextBrowser::anchorClicked, this, &PageContentContentWidget::onAnchorClicked);

	addEmojiResourcesToDocument(ui_.summary->document());
	addEmojiResourcesToDocument(ui_.description->document());

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

	if (ec != Rpc::ec_success)
	{
		ui_.titleLabel->setText("Content not found");
		ui_.metaInfoLabel->setVisible(false);
		ui_.downloadButton->setVisible(false);
		ui_.summary->setVisible(false);
		ui_.description->setVisible(false);
		return;
	}

	std::ostringstream metaInfo;

	metaInfo << ci.user << " " << ci.upTime << "\n";
	metaInfo << "\nSupported Engine Versions:\n";
	metaInfo << ci.engineName << " " << ci.engineVersion << "\n";
	metaInfo << "\nId:\n" << ci.id << "\n";

	if (!ci.parentId.empty()) {
		metaInfo << "\nParent Id:\n" << ci.parentId << "\n";
	}

	std::vector<std::string> versions;
	boost::split(versions, ci.engineVersion, boost::is_any_of("|"));

	ui_.titleLabel->setText(ci.title.c_str());
	ui_.metaInfoLabel->setText(metaInfo.str().c_str());

	refreshRelatedVersions(5);

	if (boost::istarts_with(ci.desc, "<!SUMMARY>"))
	{
		size_t p = ci.desc.find("<!DESCRIPTION>", 10);

		if (p != std::string::npos) {
			ui_.summary->setHtml(ci.desc.substr(10, p - 10).c_str());
			ui_.description->setHtml(ci.desc.substr(p + 14).c_str());
			ui_.description->setVisible(true);
		}
		else {
			ui_.summary->setHtml(ci.desc.substr(10).c_str());
			ui_.description->setVisible(false);
		}
	}
	else if (boost::istarts_with(ci.desc, "<!DOCTYPE HTML"))
	{
		ui_.summary->setHtml(ci.desc.c_str());
		ui_.description->setVisible(false);
	}
	else
	{
		ui_.summary->setText(ci.desc.c_str());
		ui_.description->setVisible(false);
	}

	if (versions.size()) {
		supportedEngineVersion_.first = ci.engineName.c_str();
		supportedEngineVersion_.second = versions[0].c_str();
	}

	videos_.clear();

	if (!ci.video.empty()) {
		std::istringstream is(ci.video);
		std::string line;
		while (std::getline(is, line)) {
			boost::trim(line);
			if (!line.empty()) {
				videos_.append(line.c_str());
			}
		}
	}

	screenshots_.clear();
	screenshots_.resize(ci.imageCount - 1);

	int downloadCount = 0;

	context_->session->queryDownloadCount(ci.id, downloadCount);

	ui_.downloadCountLabel->setText(QString("%1 downloads").arg(downloadCount));

	initView();

	for (int i = 1; i < ci.imageCount; ++i) {
		context_->contentImageLoader->load(ci.id.c_str(), i);
	}

	if (videos_.count() > 0)
	{
		videoSnapshot_ = 0;
		takeVideoSnapshot();

		presentVideo(videos_[0]);
	}

	ui_.commentWidget->setContext(context_);

	QString parentId = QString::fromStdString(ci.parentId);

	ui_.commentWidget->setTargetId(parentId.isEmpty() ? contentId_ : parentId);

	repaint();
}

void PageContentContentWidget::cancel()
{
	VideoPlayerWidget* player;
	if ((player = findVideoPlayerWidget(0)) != 0) {
		player->stop();
	}
}

void PageContentContentWidget::restore()
{
	VideoPlayerWidget* player;
	if ((player = findVideoPlayerWidget(0)) != 0) {
		//player->play();
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
		if (ui_.viewWidget->rect().contains(ui_.viewWidget->mapFrom(this, e->pos())))
		{
			QPoint pos = thumbnailWidget_->mapFrom(this, e->pos());

			if (thumbnailWidget_->rect().contains(pos))
			{
				QWidget* w = thumbnailWidget_->childAt(pos);
				const int index = thumbnailLayout_->indexOf(w);

				if (index >= 0 && index < videos_.count() + screenshots_.count())
				{
					if (index >= videos_.count()) {
						presentImage(screenshots_.at(index - videos_.count()));
					}
					else if (index < videos_.count()) {
						presentVideo(videos_.at(index));
					}
				}
			}
		}
	}
	else if (e->button() == Qt::RightButton)
	{
		if (ui_.metaInfoLabel->rect().contains(ui_.metaInfoLabel->mapFrom(this, e->pos())))
		{
			QMenu m;

			QAction* actionCopyId = m.addAction("Copy Id");
			QAction* actionCopyParentId = m.addAction("Copy Parent Id");

			QAction* selectedItem = m.exec(QCursor::pos());

			if (selectedItem == actionCopyId || selectedItem == actionCopyParentId)
			{
				Rpc::ContentInfo ci;

				Rpc::ErrorCode ec = context_->session->getContentInfo(contentId_.toStdString(), ci);

				if (ec == Rpc::ec_success)
				{
					if (selectedItem == actionCopyId)
					{
						QApplication::clipboard()->setText(ci.id.c_str());
					}
					else /*if (selectedItem == actionCopyParentId)*/
					{
						QApplication::clipboard()->setText(ci.parentId.c_str());
					}
				}
				else
				{
					context_->promptRpcError(ec);
				}
			}
		}
	}
}

void PageContentContentWidget::resizeEvent(QResizeEvent* e)
{
	int left;
	int right;

	ui_.verticalLayout_4->getContentsMargins(&left, 0, &right, 0);

	int space = ui_.verticalLayout_4->spacing();

	int width = (e->size().width() - left - right - space) * (2.0f / 3.0f);

	ui_.stackedWidget->setFixedWidth(width);
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
	QSize size = ui_.summary->document()->size().toSize();

	if (ui_.summary->height() != size.height() + 12) {
		ui_.summary->setFixedHeight(size.height() + 12);
	}

	size = ui_.description->document()->size().toSize();

	if (ui_.description->height() != size.height() + 12) {
		ui_.description->setFixedHeight(size.height() + 12);
	}

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentContentWidget::onImageLoaded(const QString& id, int index, const QPixmap& pixmap)
{
	if (id == contentId_ && index > 0)
	{
		index = index - 1;

		QLayoutItem* li = thumbnailLayout_->itemAt(videos_.count() + index);
		if (li && li->widget()) {
			QLabel* label = qobject_cast<QLabel*>(li->widget());
			if (label) {
				label->setPixmap(pixmap.scaled(192, 108, Qt::KeepAspectRatio, Qt::SmoothTransformation));
			}
		}

		screenshots_[index] = pixmap;

		if (index == 0 && videos_.count() == 0) {
			presentImage(pixmap);
		}
	}
}

void PageContentContentWidget::onVideoSnapshot(const QString& filename)
{
	VideoPlayerWidget* player = findVideoPlayerWidget(1);
	if (player) {
		player->stop();
	}

	QPixmap pixmap = QPixmap(filename, "PNG").scaled(192, 108, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QPainter painter(&pixmap);
	painter.drawPixmap(0, 0, QPixmap(":/Icons/VideoMark.png"));

	boost::system::error_code ignore;
	fs::remove(fs::path(filename.toStdString()), ignore);

	QLayoutItem* li = thumbnailLayout_->itemAt(videoSnapshot_);
	if (li && li->widget()) {
		QLabel* label = qobject_cast<QLabel*>(li->widget());
		if (label) {
			label->setPixmap(pixmap);
		}
	}

	videoSnapshot_++;
	takeVideoSnapshot();
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

	if (!engineName.isEmpty() && !engineVersion.isEmpty())
	{
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

void PageContentContentWidget::onAnchorClicked(const QUrl& url)
{
	context_->openUrl(url.toString().toStdString());
}

void PageContentContentWidget::refreshRelatedVersions(int count)
{
	Rpc::ContentInfo ci;

	Rpc::ErrorCode ec = context_->session->getContentInfo(contentId_.toStdString(), ci);

	if (ec != Rpc::ec_success) {
		return;
	}

	Rpc::ContentBrowserPrx browser;

	ec = context_->session->browseContentByParentId(!ci.parentId.empty() ? ci.parentId : ci.id, browser);

	if (ec != Rpc::ec_success) {
		return;
	}

	std::ostringstream versionInfo;

	while (count > 0)
	{
		const int n = qMin(count, 20);

		Rpc::ContentItemSeq items;
		browser->next(count, items);

		if (items.size() < n)
		{
			Rpc::ContentInfo ci2;

			if (context_->session->getContentInfo(ci.parentId, ci2) == Rpc::ec_success) {
				items.push_back({ci2.id, ci2.title});
			}

			count = 0;
		}

		if (versionInfo.str().empty() && !items.empty()) {
			versionInfo << "Related versions:<br/>";
		}

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (items[i].state == "Removed") {
				continue;
			}

			Rpc::ContentInfo ci2;

			if (context_->session->getContentInfo(items[i].id, ci2) != Rpc::ec_success) {
				continue;
			}

			if (ci2.id == ci.id) {
				continue;
			}

			std::vector<std::string> pages;
			boost::split(pages, ci2.page, boost::is_any_of(","));

			if (pages.empty()) {
				continue;
			}

			percentEncode(pages[0]);

			boost::replace_all(items[i].title, "\r", " ");
			versionInfo << "<a href=\"" << "base://content/?id=" << items[i].id << "&page=" << pages[0] << "\">" << items[i].title << "</a><br/>";
		}

		count -= n;
	}

	if (count == 0) {
		versionInfo << "<a href=\"All\">Show All</a>";
	}

	ui_.versionInfoLabel->setText(versionInfo.str().c_str());
}

void PageContentContentWidget::initView()
{
	while (ui_.stackedWidget->count())
	{
		QWidget* w = ui_.stackedWidget->widget(0);

		if (qobject_cast<VideoPlayerWidget*>(w))
		{
			VideoPlayerWidget* player = qobject_cast<VideoPlayerWidget*>(w);
			player->stop();
		}

		w->deleteLater();
		ui_.stackedWidget->removeWidget(w);
	}

	while (thumbnailLayout_->count())
	{
		QLayoutItem* li = thumbnailLayout_->takeAt(0);
		li->widget()->deleteLater();
		delete li;
	}

	if (videos_.count() > 0)
	{
		ui_.stackedWidget->addWidget(new VideoPlayerWidget);

		VideoPlayerWidget* snapshotPlayer = new VideoPlayerWidget;
		ui_.stackedWidget->addWidget(snapshotPlayer);
		QObject::connect(snapshotPlayer, &VideoPlayerWidget::snapshot, this, &PageContentContentWidget::onVideoSnapshot);
	}

	if (screenshots_.count() > 0)
	{
		ImageViewerWidget* viewer = new ImageViewerWidget;
		viewer->setPixmap(QPixmap());
		ui_.stackedWidget->addWidget(viewer);
	}

	if (videos_.count() > 0 || screenshots_.count() > 0)
	{
		for (int i = 0; i < videos_.count(); ++i) {
			QLabel* label = new QLabel;
			label->setObjectName("Thumbnail");
			label->setFixedSize(192, 108);
			label->setText(QString("Video %1").arg(i + 1));
			label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			thumbnailLayout_->addWidget(label);
		}

		for (int i = 0; i < screenshots_.count(); ++i) {
			QLabel* label = new QLabel;
			label->setObjectName("Thumbnail");
			label->setFixedSize(192, 108);
			label->setText("Loading");
			label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
			thumbnailLayout_->addWidget(label);
		}

		thumbnailLayout_->addStretch(1);
	}

	if (videos_.count() + screenshots_.count() == 0) {
		ui_.viewWidget->setVisible(false);
	}
	else if (videos_.count() + screenshots_.count() == 1) {
		ui_.viewWidget->setVisible(true);
		ui_.stackedWidget->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else if (videos_.count() + screenshots_.count() > 1) {
		ui_.viewWidget->setVisible(true);
		ui_.stackedWidget->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(true);
	}
}

VideoPlayerWidget* PageContentContentWidget::findVideoPlayerWidget(int n)
{
	int count = 0;

	for (int i = 0; i < ui_.stackedWidget->count(); ++i) {
		VideoPlayerWidget* player = qobject_cast<VideoPlayerWidget*>(ui_.stackedWidget->widget(i));
		if (player && count++ == n) {
			return player;
		}
	}

	return 0;
}

ImageViewerWidget* PageContentContentWidget::findImageViewerWidget()
{
	for (int i = 0; i < ui_.stackedWidget->count(); ++i) {
		ImageViewerWidget* viewer = qobject_cast<ImageViewerWidget*>(ui_.stackedWidget->widget(i));
		if (viewer) {
			return viewer;
		}
	}

	return 0;
}

void PageContentContentWidget::presentImage(const QPixmap& pixmap)
{
	VideoPlayerWidget* player;
	if ((player = findVideoPlayerWidget(0)) != 0) {
		player->stop();
	}

	ImageViewerWidget* viewer;
	if ((viewer = findImageViewerWidget()) != 0) {
		ui_.stackedWidget->setCurrentWidget(viewer);
		viewer->setPixmap(pixmap);
	}
}

void PageContentContentWidget::presentVideo(const QString& url)
{
	VideoPlayerWidget* player;
	if ((player = findVideoPlayerWidget(0)) != 0) {
		ui_.stackedWidget->setCurrentWidget(player);
		player->openAndPlay(url);
	}
}

void PageContentContentWidget::takeVideoSnapshot()
{
	if (videoSnapshot_ < videos_.count())
	{
		VideoPlayerWidget* player = findVideoPlayerWidget(1);
		if (player) {
			player->takeSnapshot(QString::fromStdString(context_->uniquePath() + ".png"), 0);
			player->openAndPlay(videos_[videoSnapshot_]);
		}
	}
}

