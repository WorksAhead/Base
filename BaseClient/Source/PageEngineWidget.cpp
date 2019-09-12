#include "PageEngineWidget.h"
#include "PageEngineItemWidget.h"
#include "FlowLayout.h"
#include "VTabWidget.h"

#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QTime>
#include <QGridLayout>
#include <QStyleOption>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#define ITEMS_PER_REQUEST 100

PageEngineWidget::PageEngineWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	
	layout_->setMargin(0);
	layout_->setSpacing(0);

	setLayout(layout_);

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

void PageEngineWidget::onRefresh()
{
	clear();

	Rpc::EngineVersionBrowserPrx browser;

	context_->session->browseEngineVersions(false, browser);

	if (!browser) {
		return;
	}

	std::vector<Rpc::EngineVersionInfo> v;

	for (;;)
	{
		Rpc::EngineVersionSeq items;

		browser->next(ITEMS_PER_REQUEST, items);

		v.insert(v.end(), items.begin(), items.end());

		if (items.size() < ITEMS_PER_REQUEST) {
			break;
		}
	}

	if (v.empty()) {
		return;
	}

	VTabWidget* tabWidget = new VTabWidget;

	QFont font = tabWidget->labelFont();
	font.setPixelSize(14);
	tabWidget->setLabelFont(font);

	layout_->addWidget(tabWidget);

	std::map<std::string, FlowLayout*> engineNameToLayout;

	for (const Rpc::EngineVersionInfo& info : v)
	{
		FlowLayout* currentFlowLayout;

		if (engineNameToLayout.count(info.name) == 0)
		{
			QWidget* widget = new QWidget;

			widget->setObjectName("FlowWidget");

			currentFlowLayout = new FlowLayout(10, 20, 20);

			widget->setLayout(currentFlowLayout);

			QScrollArea* scrollArea = new QScrollArea;

			scrollArea->setWidget(widget);
			scrollArea->setWidgetResizable(true);

			tabWidget->addTab(info.name.c_str(), scrollArea);

			engineNameToLayout.insert(std::make_pair(info.name, currentFlowLayout));
		}
		else
		{
			currentFlowLayout = engineNameToLayout[info.name];
		}

		PageEngineItemWidget* w = new PageEngineItemWidget(context_);

		w->setEngineVersion(qMakePair(info.name.c_str(), info.version.c_str()));

		currentFlowLayout->addWidget(w);
	}
}

void PageEngineWidget::clear()
{
	while (layout_->count())
	{
		QLayoutItem* li = layout_->takeAt(0);
		li->widget()->deleteLater();
		delete li;
	}
}

