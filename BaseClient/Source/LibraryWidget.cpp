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

void LibraryWidget::addDownloadedContent(const QString& id)
{
	libraryContent_->addContent(id);
}

