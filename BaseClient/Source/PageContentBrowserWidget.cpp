#include "PageContentBrowserWidget.h"
#include "PageContentItemWidget.h"
#include "ContentImageLoader.h"

#include <QPainter>
#include <QStyleOption>
#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QLabel>

#define ITEMS_FIRST_REQUEST 60
#define ITEMS_PER_REQUEST 20

PageContentBrowserWidget::PageContentBrowserWidget(ContextPtr context, const QString& name, const QString& category, const QString& search, QWidget* parent)
	: context_(context), name_(name), category_(category), search_(search), QWidget(parent)
{
	contentsLayout_ = new FlowLayout(0, 12, 12);

	QWidget* contentsWidget = new QWidget;
	contentsWidget->setObjectName("FlowWidget");
	contentsWidget->setLayout(contentsLayout_);

	scrollArea_ = new QScrollArea;
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea_->setWidget(contentsWidget);
	scrollArea_->setWidgetResizable(true);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(scrollArea_, 1);
	setLayout(layout);

	timer_ = new QTimer(this);
	timer_->setInterval(30);

	QObject::connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageContentBrowserWidget::onScroll);
	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &PageContentBrowserWidget::onImageLoaded);
	QObject::connect(timer_, &QTimer::timeout, this, &PageContentBrowserWidget::onTimeout);

	browser_ = 0;
	firstShow_ = true;
	count_ = 0;

	coverSize_ = 0;
}

PageContentBrowserWidget::~PageContentBrowserWidget()
{
}

void PageContentBrowserWidget::refresh()
{
	clear();

	scrollArea_->verticalScrollBar()->setValue(0);

	if (name_.endsWith('*')) {
		context_->session->browseContent("", category_.toStdString(), search_.toStdString(), browser_);
	}
	else {
		context_->session->browseContent(name_.toStdString(), category_.toStdString(), search_.toStdString(), browser_);
	}

	if (browser_) {
		showMore(ITEMS_FIRST_REQUEST);
	}
}

void PageContentBrowserWidget::setCoverSize(int n)
{
	coverSize_ = n;

	for (int i = 0; i < contentsLayout_->count(); ++i) {
		PageContentItemWidget* pi = qobject_cast<PageContentItemWidget*>(contentsLayout_->itemAt(i)->widget());
		if (pi) {
			pi->setSize(n);
		}
	}
}

void PageContentBrowserWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QPoint pos = scrollArea_->mapFrom(this, e->pos());

		if (scrollArea_->rect().contains(pos)) {
			QWidget* w = scrollArea_->childAt(pos);
			PageContentItemWidget* pi = qobject_cast<PageContentItemWidget*>(w);
			if (pi) {
				Q_EMIT contentClicked(pi->id());
			}
		}
	}
}

void PageContentBrowserWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		refresh();
		firstShow_ = false;
	}
}

void PageContentBrowserWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentBrowserWidget::onScroll(int position)
{
	QScrollBar* bar = scrollArea_->verticalScrollBar();

	if (bar->value() >= bar->maximum() / 10 * 9)
	{
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void PageContentBrowserWidget::onImageLoaded(const QString& id, int index, const QPixmap& image)
{
	if (index == 0) {
		PageContentItemWidget* pi = items_.value(id, 0);
		if (pi) {
			pi->setBackground(image);
		}
	}
}

void PageContentBrowserWidget::onTimeout()
{
	if (count_ > 0 && browser_ != 0)
	{
		const int n = qMin(count_, 5);
		int m = 0;

		Rpc::ContentItemSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::ContentItem& item = items.at(i);

			if (item.state != "Normal") {
				continue;
			}

			PageContentItemWidget* pi = new PageContentItemWidget(this);
			pi->setSize(coverSize_);
			pi->setId(item.id.c_str());
			pi->setText(item.title.c_str());
			items_.insert(item.id.c_str(), pi);
			contentsLayout_->addWidget(pi);
			context_->contentImageLoader->load(item.id.c_str(), 0);

			++m;
		}

		count_ -= m;

		if (items.size() < n) {
			browser_ = 0;
			count_ = 0;
			timer_->stop();
		}
	}
}

void PageContentBrowserWidget::clear()
{
	items_.clear();

	for (;;) {
		QLayoutItem* li = contentsLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}
}

void PageContentBrowserWidget::showMore(int n)
{
	count_ += n;
	timer_->start();
}

