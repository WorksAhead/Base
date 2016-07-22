#include "PageWidget.h"
#include "PageItemWidget.h"
#include "FlowLayout.h"
#include "SubmitContentDialog.h"
#include "ASyncDownloadTask.h"
#include "ContentWidget.h"

#include <QPainter>
#include <QScrollBar>
#include <QTime>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

#define ITEMS_PER_REQUEST 20

PageWidget::PageWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	ui_.backButton->setVisible(false);

	Rpc::StringSeq categories;
	context_->session->getCategories(categories);

	for (const std::string& category : categories) {
		ui_.categoryBox->addItem(category.c_str());
	}

	flowLayout_ = new FlowLayout(0);

	content_ = new ContentWidget(context_);

	QWidget* flowWidget = new QWidget;
	flowWidget->setObjectName("FlowWidget");
	flowWidget->setLayout(flowLayout_);

	ui_.scrollArea1->setWidget(flowWidget);
	ui_.scrollArea2->setWidget(content_);

	timer_ = new QTimer(this);

	QObject::connect(timer_, &QTimer::timeout, this, &PageWidget::onTick);

	QObject::connect(ui_.scrollArea1->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageWidget::onScroll);
	QObject::connect(ui_.backButton, &QPushButton::clicked, this, &PageWidget::onBack);
	QObject::connect(ui_.categoryBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PageWidget::onRefresh);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageWidget::onRefresh);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &PageWidget::submit);

	firstShow_ = true;
}

PageWidget::~PageWidget()
{
	cancelAllImageLoadTasks();
}

void PageWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (ui_.stackedWidget->currentIndex() != 0) {
			return;
		}

		QPoint pos = ui_.scrollArea1->mapFrom(this, e->pos());
		if (ui_.scrollArea1->rect().contains(pos))
		{
			QWidget* w = ui_.scrollArea1->childAt(pos);
			PageItemWidget* pi = qobject_cast<PageItemWidget*>(w);
			if (pi)
			{
				Rpc::ContentInfo ci;
				Rpc::ErrorCode ec = context_->session->getContentInfo(pi->id().toStdString(), ci);
				if (ec != Rpc::ec_success) {
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

				content_->setContentId(ci.id.c_str());
				content_->setTitle(ci.title.c_str());
				content_->setSummary(summary.str().c_str());
				content_->setDescription(ci.desc.c_str());

				content_->setEngineVersionCount(versions.size());

				for (int i = 0; i < versions.size(); ++i) {
					content_->setEngineVersion(i, ci.engineName.c_str(), versions[i].c_str());
				}

				content_->setImageCount(ci.imageCount - 1);

				for (int i = 1; i < ci.imageCount; ++i) {
					loadImage(ImageIndex(ci.id.c_str(), i), true);
				}

				ui_.scrollArea2->verticalScrollBar()->setValue(0);

				ui_.backButton->setVisible(true);
				ui_.categoryBox->setVisible(false);
				ui_.refreshButton->setVisible(false);
				ui_.submitButton->setVisible(false);
				ui_.stackedWidget->setCurrentIndex(1);
			}
		}
	}
}

void PageWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		timer_->start(50);
		firstShow_ = false;
	}
}

void PageWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageWidget::onScroll(int position)
{
	if (position == ui_.scrollArea1->verticalScrollBar()->maximum()) {
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void PageWidget::onBack()
{
	ui_.backButton->setVisible(false);
	ui_.categoryBox->setVisible(true);
	ui_.refreshButton->setVisible(true);
	ui_.submitButton->setVisible(true);

	ui_.stackedWidget->setCurrentIndex(0);
}

void PageWidget::onRefresh()
{
	clear();
	cancelAllImageLoadTasks();

	ui_.scrollArea1->verticalScrollBar()->setValue(0);

	QString category;
	if (ui_.categoryBox->currentIndex() != 0) {
		category = ui_.categoryBox->currentText();
	}

	context_->session->browseContent(name_.toStdString(), category.toStdString(), browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void PageWidget::onTick()
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

void PageWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = qMin(count, ITEMS_PER_REQUEST);

		Rpc::ContentItemSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::ContentItem& item = items.at(i);

			PageItemWidget* pi = new PageItemWidget(this);
			pi->setFixedSize(QSize(300, 300));
			pi->setId(item.id.c_str());
			pi->setText(item.title.c_str());
			pageItems_.insert(item.id.c_str(), pi);
			flowLayout_->addWidget(pi);
			loadImage(ImageIndex(item.id.c_str(), 0));
		}

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

void PageWidget::submit()
{
	SubmitContentDialog d(context_, this);
	d.setPage(name_);
	d.exec();
}

void PageWidget::clear()
{
	pageItems_.clear();

	for (;;) {
		QLayoutItem* li = flowLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}
}

QString PageWidget::makeImageFilename(const ImageIndex& imageIndex)
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

void PageWidget::cancelAllImageLoadTasks()
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

void PageWidget::loadImage(const ImageIndex& imageIndex, bool highPriority)
{
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

void PageWidget::setImage(const ImageIndex& imageIndex)
{
	if (imageIndex.second == 0) {
		PageItemWidget* pi = pageItems_.value(imageIndex.first, 0);
		if (pi) {
			pi->setBackground(QPixmap(makeImageFilename(imageIndex), "JPG"));
		}
	}
	else if (imageIndex.second > 0) {
		if (imageIndex.first == content_->contentId()) {
			content_->setImage(imageIndex.second - 1, QPixmap(makeImageFilename(imageIndex), "JPG"));
		}
	}
}

