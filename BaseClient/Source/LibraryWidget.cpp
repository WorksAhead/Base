#include "LibraryWidget.h"
#include "VTabWidget.h"
#include "LibraryContentWidget.h"

#include <QBoxLayout>
#include <QTextEdit>

LibraryWidget::LibraryWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	libraryContent_ = new LibraryContentWidget(context_);

	VTabWidget* p = new VTabWidget;
	p->addTab("Content", libraryContent_);
	p->setCurrentIndex(0);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(p);
	setLayout(layout);
}

LibraryWidget::~LibraryWidget()
{
}

void LibraryWidget::addContent(const QString& id)
{
	libraryContent_->addContent(id);
}

void LibraryWidget::removeContent(const QString& id)
{
	libraryContent_->removeContent(id);
}

void LibraryWidget::addProject(const QString& id)
{
	libraryContent_->addProject(id);
}

void LibraryWidget::removeProject(const QString& id)
{
	libraryContent_->removeProject(id);
}

