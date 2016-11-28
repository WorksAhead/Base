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

#define ITEMS_PER_REQUEST 20

PageContentBrowserWidget::PageContentBrowserWidget(ContextPtr context, const QString& name, const QString& category, QWidget* parent)
	: context_(context), name_(name), category_(category), QWidget(parent)
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

	QObject::connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageContentBrowserWidget::onScroll);
	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &PageContentBrowserWidget::onImageLoaded);

	browser_ = 0;
	firstShow_ = true;
}

PageContentBrowserWidget::~PageContentBrowserWidget()
{
}

void PageContentBrowserWidget::refresh()
{
	clear();

	scrollArea_->verticalScrollBar()->setValue(0);

	if (name_.endsWith('*')) {
		context_->session->browseContent("", category_.toStdString(), browser_);
	}
	else {
		context_->session->browseContent(name_.toStdString(), category_.toStdString(), browser_);
	}

	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
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
	if (position == scrollArea_->verticalScrollBar()->maximum()) {
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

void PageContentBrowserWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = qMin(count, ITEMS_PER_REQUEST);

		Rpc::ContentItemSeq items;
		browser_->next(n, items);

		for (int i = 0; i < items.size(); ++i)
		{
			const Rpc::ContentItem& item = items.at(i);

			PageContentItemWidget* pi = new PageContentItemWidget(this);
			pi->setFixedSize(QSize(230, 230));
			pi->setId(item.id.c_str());
			pi->setText(item.title.c_str());
			items_.insert(item.id.c_str(), pi);
			contentsLayout_->addWidget(pi);
			context_->contentImageLoader->load(item.id.c_str(), 0);
		}

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

