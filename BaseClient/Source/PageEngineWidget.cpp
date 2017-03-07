#include "PageEngineWidget.h"
#include "PageEngineItemWidget.h"
#include "FlowLayout.h"
#include "VTabWidget.h"

#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QTime>
#include <QGridLayout>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

//namespace fs = boost::filesystem;

#define ITEMS_PER_REQUEST 100

// Rpc::EngineVersionBrowserPrx browser_;

PageEngineWidget::PageEngineWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	
	layout_->setMargin(0);
	layout_->setSpacing(0);

	setLayout(layout_);

	/*ui_.setupUi(this);

	QWidget* w = new QWidget;
	w->setObjectName("FlowWidget");

	flowLayout_ = new FlowLayout(0, 20, 20);

	w->setLayout(flowLayout_);

	ui_.scrollArea->setWidget(w);

	QObject::connect(ui_.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageEngineWidget::onScroll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageEngineWidget::onRefresh);*/

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

	Rpc::EngineVersionBrowserPrx browser_;

	context_->session->browseEngineVersions(false, browser_);

	std::vector<Rpc::EngineVersionInfo> v;

	for (;;)
	{
		Rpc::EngineVersionSeq items;

		browser_->next(ITEMS_PER_REQUEST, items);

		v.insert(v.end(), items.begin(), items.end());

		if (items.size() < ITEMS_PER_REQUEST) {
			break;
		}
	}

	std::stable_sort(v.begin(), v.end(), [](auto lhs, auto rhs) -> bool
	{
		return (lhs.name < rhs.name);
	});

	VTabWidget* tabWidget = new VTabWidget;

	QFont font = tabWidget->labelFont();
	font.setPixelSize(14);
	tabWidget->setLabelFont(font);

	layout_->addWidget(tabWidget);

	std::string lastName;

	QWidget* lastWidget;
	FlowLayout* lastFlowLayout;
	QScrollArea* lastScrollArea;

	for (const Rpc::EngineVersionInfo& info : v)
	{
		if (info.name != lastName)
		{
			lastWidget = new QWidget;

			lastWidget->setObjectName("FlowWidget");

			lastFlowLayout = new FlowLayout(10, 20, 20);

			lastWidget->setLayout(lastFlowLayout);

			lastScrollArea = new QScrollArea;

			lastScrollArea->setWidget(lastWidget);
			lastScrollArea->setWidgetResizable(true);

			tabWidget->addTab(info.name.c_str(), lastScrollArea);

			lastName = info.name;
		}

		PageEngineItemWidget* w = new PageEngineItemWidget(context_);

		w->setEngineVersion(qMakePair(info.name.c_str(), info.version.c_str()));

		lastFlowLayout->addWidget(w);
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

