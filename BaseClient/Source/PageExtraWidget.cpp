#include "PageExtraWidget.h"
#include "PageExtraItemWidget.h"
#include "FlowLayout.h"
#include "ExtraImageLoader.h"

#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QTime>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenu>
#include <QClipboard>

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

	flowLayout_ = new FlowLayout(0, 12, 12);

	w->setLayout(flowLayout_);

	ui_.scrollArea->setWidget(w);

	timer_ = new QTimer(this);
	timer_->setInterval(30);

	QObject::connect(ui_.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &PageExtraWidget::onScroll);
	QObject::connect(ui_.backButton, &QPushButton::clicked, this, &PageExtraWidget::onBack);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageExtraWidget::onRefresh);
	QObject::connect(ui_.installButton, &QPushButton::clicked, this, &PageExtraWidget::onInstall);
	QObject::connect(ui_.filterWidget->labelSelectorWidget(), &LabelSelectorWidget::clicked, this, &PageExtraWidget::onCategoryChanged);
	QObject::connect(context_->extraImageLoader, &ExtraImageLoader::loaded, this, &PageExtraWidget::onImageLoaded);
	QObject::connect(timer_, &QTimer::timeout, this, &PageExtraWidget::onTimeout);
	QObject::connect(ui_.description, &QTextBrowser::anchorClicked, this, &PageExtraWidget::onAnchorClicked);

	ui_.backButton->setVisible(false);

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
		if (ui_.stackedWidget->currentIndex() == 0)
		{
			QPoint pos = ui_.scrollArea->mapFrom(this, e->pos());

			if (ui_.scrollArea->rect().contains(pos))
			{
				QWidget* w = ui_.scrollArea->childAt(pos);

				PageExtraItemWidget* pi = qobject_cast<PageExtraItemWidget*>(w);

				if (pi)
				{
					Rpc::ExtraInfo item;

					if (context_->session->getExtraInfo(pi->id().toStdString(), item) == Rpc::ec_success)
					{
						ui_.titleLabel->setText(pi->text());

						std::ostringstream metaInfo;

						metaInfo << item.user << " " << item.uptime << "\n";
						metaInfo << "\nId:\n" << item.id << "\n";

						if (!item.parentId.empty()) {
							metaInfo << "\nParent Id:\n" << item.parentId << "\n";
						}

						ui_.metaInfoLabel->setText(metaInfo.str().c_str());

						if (boost::istarts_with(item.info, "<!DOCTYPE HTML")) {
							ui_.description->setHtml(item.info.c_str());
						}
						else {
							ui_.description->setText(item.info.c_str());
						}

						ui_.stackedWidget->setCurrentIndex(1);
						ui_.backButton->setVisible(true);
						ui_.refreshButton->setVisible(false);

						currentId_ = pi->id();

						ui_.commentWidget->setContext(context_);

						QString parentId = QString::fromStdString(item.parentId);

						ui_.commentWidget->setTargetId(parentId.isEmpty() ? pi->id() : parentId);

						ui_.scrollArea_2->verticalScrollBar()->setValue(0);

						repaint();
					}
				}
			}
		}
	}
	else if (e->button() == Qt::RightButton)
	{
		if (ui_.stackedWidget->currentIndex() == 1)
		{
			if (ui_.metaInfoLabel->rect().contains(ui_.metaInfoLabel->mapFrom(this, e->pos())))
			{
				QMenu m;

				QAction* actionCopyId = m.addAction("Copy Id");
				QAction* actionCopyParentId = m.addAction("Copy Parent Id");

				QAction* selectedItem = m.exec(QCursor::pos());

				if (selectedItem == actionCopyId || selectedItem == actionCopyParentId)
				{
					Rpc::ExtraInfo info;

					Rpc::ErrorCode ec = context_->session->getExtraInfo(currentId_.toStdString(), info);

					if (ec == Rpc::ec_success)
					{
						if (selectedItem == actionCopyId)
						{
							QApplication::clipboard()->setText(info.id.c_str());
						}
						else /*if (selectedItem == actionCopyParentId)*/
						{
							QApplication::clipboard()->setText(info.parentId.c_str());
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
	QSize size = ui_.description->document()->size().toSize();

	if (ui_.description->height() != size.height() + 12) {
		ui_.description->setFixedHeight(size.height() + 12);
	}

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

void PageExtraWidget::onBack()
{
	ui_.stackedWidget->setCurrentIndex(0);
	ui_.backButton->setVisible(false);
	ui_.refreshButton->setVisible(true);
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

void PageExtraWidget::onInstall()
{
	context_->installExtra(currentId_.toStdString());
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

void PageExtraWidget::onAnchorClicked(const QUrl& url)
{
	context_->openUrl(url.toString().toStdString());
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

