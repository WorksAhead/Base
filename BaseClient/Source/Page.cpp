#include "Page.h"
#include "PageItem.h"
#include "FlowLayout.h"
#include "SubmitContentDialog.h"
#include "ASyncDownloadTask.h"

#include <QPainter>
#include <QScrollBar>
#include <QTime>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#define ITEMS_PER_REQUEST 20

Page::Page(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	Rpc::StringSeq categories;
	context_->session->getCategories(categories);

	for (const std::string& category : categories) {
		ui_.categoryBox->addItem(category.c_str());
	}

	flowLayout_ = new FlowLayout;

	QWidget* w = new QWidget;
	w->setLayout(flowLayout_);
	ui_.scrollArea->setWidget(w);

	timer_ = new QTimer(this);

	QObject::connect(timer_, &QTimer::timeout, this, &Page::onTick);

	QObject::connect(ui_.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &Page::onSliderMoved);
	QObject::connect(ui_.categoryBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Page::onRefresh);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &Page::onRefresh);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &Page::submit);

	firstShow_ = true;
}

Page::~Page()
{
	QMapIterator<QString, ASyncDownloadTask*> it(coverDownloadTasks_);
	while (it.hasNext()) {
		it.next();
		ASyncDownloadTask* task = it.value();
		task->cancel();
		delete task;
	}
}

void Page::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		timer_->start(50);
		firstShow_ = false;
	}
}

void Page::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Page::onSliderMoved(int position)
{
	if (position == ui_.scrollArea->verticalScrollBar()->maximum()) {
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void Page::onRefresh()
{
	clear();

	ui_.scrollArea->verticalScrollBar()->setValue(0);

	QString category;
	if (ui_.categoryBox->currentIndex() != 0) {
		category = ui_.categoryBox->currentText();
	}

	context_->session->browseContent(name_.toStdString(), category.toStdString(), browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void Page::onTick()
{
	const int maxTasks = 5;

	QTime t;
	t.start();

	int index = 0;

	while (index < qMin(maxTasks, pendingCovers_.count()))
	{
		const QString& id = pendingCovers_.at(index);
		if (loadedCovers_.contains(id)) {
			PageItem* pi = pageItems_.value(id, 0);
			if (pi) {
				pi->setBackground(QPixmap(makeCoverFilename(id), "JPG"));
			}
			pendingCovers_.removeAt(index);
			continue;
		}

		ASyncDownloadTask* task = coverDownloadTasks_.value(id, 0);

		if (task)
		{
			const int state = task->state();

			if (state == ASyncTask::state_finished) {
				PageItem* pi = pageItems_.value(id, 0);
				if (pi) {
					pi->setBackground(QPixmap(task->filename().c_str(), "JPG"));
				}
				loadedCovers_.insert(id);
			}

			if (state == ASyncTask::state_finished || state == ASyncTask::state_failed) {
				delete task;
				coverDownloadTasks_.remove(id);
				pendingCovers_.removeAt(index);
				continue;
			}
		}
		else
		{
			Rpc::DownloaderPrx downloader;

			Rpc::ErrorCode ec = context_->session->downloadContentImage(id.toStdString(), 0, downloader);
			if (ec != Rpc::ec_success) {
				pendingCovers_.removeAt(index);
				continue;
			}

			task = new ASyncDownloadTask(context_, downloader);
			task->setFilename(makeCoverFilename(id).toStdString());
			task->start();
			coverDownloadTasks_.insert(id, task);
		}

		if (t.elapsed() > 10) {
			break;
		}

		++index;
	}
}

void Page::showMore(int count)
{
	while (count > 0)
	{
		const int n = qMin(count, ITEMS_PER_REQUEST);

		Rpc::ContentItemSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::ContentItem& item = items.at(i);

			PageItem* pi = new PageItem(this);
			pi->setFixedSize(QSize(300, 300));
			pi->setId(item.id.c_str());
			pi->setText(item.title.c_str());
			pageItems_.insert(item.id.c_str(), pi);
			flowLayout_->addWidget(pi);
			loadCover(item.id.c_str());
		}

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

void Page::submit()
{
	SubmitContentDialog d(context_, this);
	d.setPage(name_);
	d.exec();
}

void Page::clear()
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

void Page::loadCover(const QString& id)
{
	if (loadedCovers_.contains(id)) {
		PageItem* pi = pageItems_.value(id, 0);
		if (pi) {
			pi->setBackground(QPixmap(makeCoverFilename(id), "JPG"));
		}
		return;
	}

	pendingCovers_.append(id);
}

QString Page::makeCoverFilename(const QString& id)
{
	fs::path path = context_->cachePath();
	path /= id.toStdString() + "_cover.jpg";
	return QString(path.string().c_str());
}

