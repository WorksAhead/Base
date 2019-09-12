#include "LibraryExtraWidget.h"
#include "LibraryExtraItemWidget.h"
#include "QtUtils.h"

#include <QScrollArea>
#include <QPainter>
#include <QGridLayout>
#include <QStyleOption>

#include <algorithm>

LibraryExtraWidget::LibraryExtraWidget(ContextPtr context, QWidget* parent) : context_(context), QWidget(parent)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* w = new QWidget;

	ui_.setupUi(w);

	extrasLayout_ = new FlowLayout(0, 20, 20);

	ui_.extrasWidget->setLayout(extrasLayout_);

	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(w);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(scrollArea);

	setLayout(layout);

	firstShow_ = true;
}

LibraryExtraWidget::~LibraryExtraWidget()
{
}

void LibraryExtraWidget::addExtra(const QString& id)
{
	Rpc::ExtraInfo info;
	Rpc::ErrorCode ec = context_->session->getExtraInfo(id.toStdString(), info);

	LibraryExtraItemWidget* w = new LibraryExtraItemWidget(context_);
	w->setId(id);
	w->setTitle(info.title.c_str());

	int idx = 0;
	while (idx < extrasLayout_->count()) {
		LibraryExtraItemWidget* w = (LibraryExtraItemWidget*)extrasLayout_->itemAt(idx)->widget();
		if (QString(info.title.c_str()) < w->getTitle()) {
			break;
		}
		++idx;
	}

	extrasLayout_->insertWidget(idx, w);

	extraItemWidgets_.insert(id, w);
}

void LibraryExtraWidget::removeExtra(const QString& id)
{
	LibraryExtraItemWidget* w = extraItemWidgets_.value(id, 0);
	if (w) {
		w->deleteLater();
		extrasLayout_->removeWidget(w);
		extraItemWidgets_.remove(id);
	}
}

void LibraryExtraWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		refresh();
		firstShow_ = false;
	}
}

void LibraryExtraWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryExtraWidget::refresh()
{
	for (;;) {
		QLayoutItem* li = extrasLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}

	extraItemWidgets_.clear();

	std::vector<std::string> downloadedExtraList;
	context_->getDownloadedExtraList(downloadedExtraList);

	typedef std::pair<std::string, std::string> IdTitlePair;

	std::vector<IdTitlePair> list(downloadedExtraList.size());

	for (size_t i = 0; i < list.size(); ++i) {
		list[i].first = downloadedExtraList[i];
		Rpc::ExtraInfo info;
		context_->session->getExtraInfo(list[i].first, info);
		list[i].second = toLocal8bit(info.title);
	}

	std::sort(list.begin(), list.end(), [](const IdTitlePair& lhs, const IdTitlePair& rhs){
		return lhs.second < rhs.second;
	});

	for (size_t i = 0; i < list.size(); ++i) {
		LibraryExtraItemWidget* w = new LibraryExtraItemWidget(context_);
		w->setId(list[i].first.c_str());
		w->setTitle(QString::fromLocal8Bit(list[i].second.c_str()));
		extrasLayout_->addWidget(w);
		extraItemWidgets_.insert(list[i].first.c_str(), w);
	}
}

