#include "PageContentWidget.h"
#include "PageItemWidget.h"
#include "FlowLayout.h"
#include "SubmitContentDialog.h"
#include "ASyncDownloadTask.h"
#include "ContentWidget.h"
#include "ContentImageLoader.h"

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

PageContentWidget::PageContentWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
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

	QObject::connect(ui_.scrollArea1->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageContentWidget::onScroll);
	QObject::connect(ui_.backButton, &QPushButton::clicked, this, &PageContentWidget::onBack);
	QObject::connect(ui_.categoryBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PageContentWidget::onRefresh);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageContentWidget::onRefresh);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &PageContentWidget::submit);

	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &PageContentWidget::onImageLoaded);

	firstShow_ = true;
}

PageContentWidget::~PageContentWidget()
{
}

void PageContentWidget::mousePressEvent(QMouseEvent* e)
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
					context_->contentImageLoader->load(ci.id.c_str(), i, true);
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

void PageContentWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void PageContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentWidget::onScroll(int position)
{
	if (position == ui_.scrollArea1->verticalScrollBar()->maximum()) {
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void PageContentWidget::onBack()
{
	ui_.backButton->setVisible(false);
	ui_.categoryBox->setVisible(true);
	ui_.refreshButton->setVisible(true);
	ui_.submitButton->setVisible(true);

	ui_.stackedWidget->setCurrentIndex(0);
}

void PageContentWidget::onRefresh()
{
	clear();

	ui_.scrollArea1->verticalScrollBar()->setValue(0);

	QString category;
	if (ui_.categoryBox->currentIndex() != 0) {
		category = ui_.categoryBox->currentText();
	}

	if (name_.endsWith('*')) {
		context_->session->browseContent("", category.toStdString(), browser_);
	}
	else {
		context_->session->browseContent(name_.toStdString(), category.toStdString(), browser_);
	}

	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void PageContentWidget::onImageLoaded(const QString& id, int index, const QPixmap& image)
{
	if (index == 0) {
		PageItemWidget* pi = pageItems_.value(id, 0);
		if (pi) {
			pi->setBackground(image);
		}
	}
	else if (index > 0) {
		if (id == content_->contentId()) {
			content_->setImage(index - 1, image);
		}
	}
}

void PageContentWidget::showMore(int count)
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
			context_->contentImageLoader->load(item.id.c_str(), 0);
		}

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

void PageContentWidget::submit()
{
	SubmitContentDialog d(context_, this);
	d.setPage(name_);
	d.exec();
}

void PageContentWidget::clear()
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
