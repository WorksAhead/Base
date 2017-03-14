#include "PageExtraWidget.h"
#include "PageExtraItemWidget.h"
#include "FlowLayout.h"
#include "ExtraImageLoader.h"

#include <QPainter>
#include <QScrollBar>
#include <QTime>
#include <QMouseEvent>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

#define ITEMS_FIRST_REQUEST 60
#define ITEMS_PER_REQUEST 20

PageExtraWidget::PageExtraWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	QWidget* w = new QWidget;
	w->setObjectName("FlowWidget");

	flowLayout_ = new FlowLayout(0, 20, 20);

	w->setLayout(flowLayout_);

	ui_.scrollArea->setWidget(w);

	timer_ = new QTimer(this);
	timer_->setInterval(30);

	QObject::connect(ui_.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageExtraWidget::onScroll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageExtraWidget::onRefresh);
	QObject::connect(ui_.filterWidget->labelSelectorWidget(), &LabelSelectorWidget::clicked, this, &PageExtraWidget::onCategoryChanged);
	QObject::connect(context_->extraImageLoader, &ExtraImageLoader::loaded, this, &PageExtraWidget::onImageLoaded);
	QObject::connect(timer_, &QTimer::timeout, this, &PageExtraWidget::onTimeout);

	firstShow_ = true;
	count_ = 0;
}

PageExtraWidget::~PageExtraWidget()
{
}

CategoryFilterWidget* PageExtraWidget::categoryFilterWidget()
{
	return ui_.filterWidget;
}

void PageExtraWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QPoint pos = ui_.scrollArea->mapFrom(this, e->pos());

		if (ui_.scrollArea->rect().contains(pos))
		{
			QWidget* w = ui_.scrollArea->childAt(pos);
			PageExtraItemWidget* pi = qobject_cast<PageExtraItemWidget*>(w);
			if (pi) {
				int ret = QMessageBox::question(this, "Base", QString(tr("Do you want to install %1 ?")).arg(pi->text()));
				if (ret == QMessageBox::Yes) {
					context_->installExtra(pi->id().toStdString());
				}
			}
		}
	}
}

void PageExtraWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void PageExtraWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageExtraWidget::onCategoryChanged()
{
	onRefresh();
}

void PageExtraWidget::onScroll(int position)
{
	if (position == ui_.scrollArea->verticalScrollBar()->maximum()) {
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void PageExtraWidget::onRefresh()
{
	clear();

	ui_.scrollArea->verticalScrollBar()->setValue(0);

	context_->session->browseExtra(currentCategory().toStdString(), "", browser_);

	if (browser_) {
		showMore(ITEMS_FIRST_REQUEST);
	}
}

void PageExtraWidget::onImageLoaded(const QString& id, const QPixmap& bg)
{
	PageExtraItemWidget* item = items_.value(id);
	if (item) {
		item->setBackground(bg);
	}
}

void PageExtraWidget::onTimeout()
{
	if (count_ > 0 && browser_ != 0)
	{
		const int n = qMin(count_, 5);

		Rpc::ExtraInfoSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::ExtraInfo& item = items.at(i);

			PageExtraItemWidget* itemWidget = new PageExtraItemWidget;
			itemWidget->setId(item.id.c_str());
			itemWidget->setText(item.title.c_str());
			itemWidget->setSize(0);

			std::ostringstream tip;
			tip << item.title << "\n\n";
			tip << item.user << " " << item.uptime << "\n";
			tip << "\nID:\n" << item.id << "\n";
			tip << "\n" << item.info.c_str();

			itemWidget->setToolTip(tip.str().c_str());

			items_.insert(item.id.c_str(), itemWidget);

			flowLayout_->addWidget(itemWidget);

			context_->extraImageLoader->load(item.id.c_str());
		}

		count_ -= n;

		if (items.size() < n) {
			browser_ = 0;
			count_ = 0;
			timer_->stop();
		}
	}
}

void PageExtraWidget::showMore(int n)
{
	count_ += n;
	timer_->start();
}

void PageExtraWidget::clear()
{
	items_.clear();

	for (;;) {
		QLayoutItem* li = flowLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}
}

QString PageExtraWidget::currentCategory()
{
	QStringList list = ui_.filterWidget->labelSelectorWidget()->getSelectedLabels();

	QString category;

	for (const QString& s : list) {
		if (!category.isEmpty()) {
			category += ",";
		}
		category += s;
	}

	return category;
}

