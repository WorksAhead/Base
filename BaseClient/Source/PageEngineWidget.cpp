#include "PageEngineWidget.h"
#include "PageEngineItemWidget.h"
#include "FlowLayout.h"

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

PageEngineWidget::PageEngineWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	QWidget* w = new QWidget;
	w->setObjectName("FlowWidget");

	flowLayout_ = new FlowLayout(0, 20, 20);

	w->setLayout(flowLayout_);

	ui_.scrollArea->setWidget(w);

	QObject::connect(ui_.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageEngineWidget::onScroll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageEngineWidget::onRefresh);

	firstShow_ = true;
}

PageEngineWidget::~PageEngineWidget()
{
}

void PageEngineWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void PageEngineWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageEngineWidget::onScroll(int position)
{
	if (position == ui_.scrollArea->verticalScrollBar()->maximum()) {
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void PageEngineWidget::onRefresh()
{
	clear();

	ui_.scrollArea->verticalScrollBar()->setValue(0);

	context_->session->browseEngineVersions(false, browser_);

	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void PageEngineWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = qMin(count, ITEMS_PER_REQUEST);

		Rpc::EngineVersionSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::EngineVersionInfo& item = items.at(i);

			PageEngineItemWidget* itemWidget = new PageEngineItemWidget(context_);
			itemWidget->setEngineVersion(qMakePair(item.name.c_str(), item.version.c_str()));

			flowLayout_->addWidget(itemWidget);
		}

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

void PageEngineWidget::clear()
{
	for (;;) {
		QLayoutItem* li = flowLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}
}

