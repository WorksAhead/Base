#include "LibraryContentWidget.h"
#include "ContentItemWidget.h"
#include "ContentImageLoader.h"

#include <QScrollArea>
#include <QPainter>
#include <QGridLayout>

#include <algorithm>

LibraryContentWidget::LibraryContentWidget(ContextPtr context, QWidget* parent) : context_(context), QWidget(parent)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* w = new QWidget;

	ui_.setupUi(w);

	projectsLayout_ = new FlowLayout(0);
	contentsLayout_ = new FlowLayout(0, 20, 20);

	ui_.projestsWidget->setLayout(projectsLayout_);
	ui_.contentsWidget->setLayout(contentsLayout_);

	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(w);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(scrollArea);

	setLayout(layout);

	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &LibraryContentWidget::onImageLoaded);

	firstShow_ = true;
}

LibraryContentWidget::~LibraryContentWidget()
{
}

void LibraryContentWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void LibraryContentWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryContentWidget::onRefresh()
{
	for (;;) {
		QLayoutItem* li = contentsLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}

	contentItemWidgets_.clear();

	std::vector<std::string> downloadedContentList;
	context_->getDownloadedContentList(downloadedContentList);

	typedef std::pair<std::string, std::string> TitleIdPair;

	std::vector<TitleIdPair> contentList;

	for (const std::string& id : downloadedContentList)
	{
		Rpc::ContentInfo ci;
		Rpc::ErrorCode ec = context_->session->getContentInfo(id, ci);
		contentList.push_back(std::make_pair(ci.title, id));
	}

	std::sort(contentList.begin(), contentList.end(), [](const TitleIdPair& lhs, const TitleIdPair& rhs) -> bool {
		return lhs.first < rhs.first;
	});

	for (const TitleIdPair& p : contentList) {
		ContentItemWidget* w = new ContentItemWidget(context_);
		w->setContentId(p.second.c_str());
		w->setTitle(p.first.c_str());
		contentsLayout_->addWidget(w);
		contentItemWidgets_.insert(p.second.c_str(), w);
		context_->contentImageLoader->load(p.second.c_str(), 0);
	}
}

void LibraryContentWidget::onImageLoaded(const QString& id, int index, const QPixmap& image)
{
	ContentItemWidget* w = contentItemWidgets_.value(id, 0);
	if (w && index == 0) {
		w->setImage(image);
	}
}

