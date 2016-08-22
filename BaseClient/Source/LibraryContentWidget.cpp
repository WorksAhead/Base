#include "LibraryContentWidget.h"
#include "ProjectItemWidget.h"
#include "LibraryContentItemWidget.h"
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

	projectsLayout_ = new FlowLayout(0, 20, 20);
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

void LibraryContentWidget::addContent(const QString& id)
{
	Rpc::ContentInfo ci;
	Rpc::ErrorCode ec = context_->session->getContentInfo(id.toStdString(), ci);

	int i = 0;

	while (i < contentsLayout_->count()) {
		LibraryContentItemWidget* w = (LibraryContentItemWidget*)contentsLayout_->itemAt(i)->widget();
		if (ci.title < w->title().toStdString()) {
			break;
		}
		++i;
	}

	LibraryContentItemWidget* w = new LibraryContentItemWidget(context_);
	w->setContentId(id);
	w->setTitle(ci.title.c_str());
	contentsLayout_->insertWidget(i, w);
	contentItemWidgets_.insert(id, w);
	context_->contentImageLoader->load(id, 0);
}

void LibraryContentWidget::removeContent(const QString& id)
{
	LibraryContentItemWidget* w = contentItemWidgets_.value(id, 0);
	if (w) {
		w->deleteLater();
		contentsLayout_->removeWidget(w);
		contentItemWidgets_.remove(id);
	}
}

void LibraryContentWidget::addProject(const QString& id)
{
	ProjectInfo pi;
	if (context_->getProject(pi, id.toStdString()))
	{
		ProjectItemWidget* w = new ProjectItemWidget(context_);
		w->setContentId(pi.contentId.c_str());
		w->setProjectId(pi.id.c_str());
		w->setName(QString::fromLocal8Bit(pi.name.c_str()));
		int idx = 0;
		while (idx < projectsLayout_->count()) {
			ProjectItemWidget* w = (ProjectItemWidget*)projectsLayout_->itemAt(idx)->widget();
			if (pi.name < w->name().toLocal8Bit().data()) {
				break;
			}
			++idx;
		}
		projectsLayout_->insertWidget(idx, w);
		projectItemWidgets_.insert(pi.id.c_str(), w);
		projectItemsOfContentItem_[pi.contentId.c_str()].append(w);
		context_->contentImageLoader->load(pi.contentId.c_str(), false);
	}
}

void LibraryContentWidget::removeProject(const QString& id)
{
	ProjectItemWidget* w = projectItemWidgets_.value(id, 0);
	if (w) {
		w->deleteLater();
		projectsLayout_->removeWidget(w);
		projectItemWidgets_.remove(id);
		projectItemsOfContentItem_.find(w->contentId())->removeOne(w);
	}
}

void LibraryContentWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		refresh();
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

void LibraryContentWidget::refresh()
{
	for (;;) {
		QLayoutItem* li = contentsLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}

	for (;;) {
		QLayoutItem* li = projectsLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}

	contentItemWidgets_.clear();
	projectItemWidgets_.clear();
	projectItemsOfContentItem_.clear();

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

	for (const TitleIdPair& p : contentList)
	{
		LibraryContentItemWidget* w = new LibraryContentItemWidget(context_);
		w->setContentId(p.second.c_str());
		w->setTitle(p.first.c_str());
		contentsLayout_->addWidget(w);
		contentItemWidgets_.insert(p.second.c_str(), w);
		context_->contentImageLoader->load(p.second.c_str(), 0);
	}

	std::vector<ProjectInfo> projectList;
	context_->getProjectList(projectList);

	for (ProjectInfo& pi : projectList)
	{
		ProjectItemWidget* w = new ProjectItemWidget(context_);
		w->setContentId(pi.contentId.c_str());
		w->setProjectId(pi.id.c_str());
		w->setName(QString::fromLocal8Bit(pi.name.c_str()));
		projectsLayout_->addWidget(w);
		projectItemWidgets_.insert(pi.id.c_str(), w);
		projectItemsOfContentItem_[pi.contentId.c_str()].append(w);
		context_->contentImageLoader->load(pi.contentId.c_str(), 0);
	}
}

void LibraryContentWidget::onImageLoaded(const QString& id, int index, const QPixmap& image)
{
	LibraryContentItemWidget* w = contentItemWidgets_.value(id, 0);
	if (w && index == 0) {
		w->setImage(image);
	}

	auto it = projectItemsOfContentItem_.find(id);
	if (it != projectItemsOfContentItem_.end()) {
		QList<ProjectItemWidget*>& list = *it;
		for (ProjectItemWidget* p : list) {
			p->setImage(image);
		}
	}
}

