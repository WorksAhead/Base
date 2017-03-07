#include "LibraryWidget.h"
#include "VTabWidget.h"
#include "LibraryContentWidget.h"
#include "LibraryEngineWidget.h"
#include "LibraryExtraWidget.h"

#include <QBoxLayout>
#include <QPainter>
#include <QTextEdit>

LibraryWidget::LibraryWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	libraryContent_ = new LibraryContentWidget(context_);
	libraryEngine_ = new LibraryEngineWidget(context_);
	libraryExtra_ = new LibraryExtraWidget(context_);

	tabWidget_ = new VTabWidget;

	QFont font = tabWidget_->labelFont();
	font.setPixelSize(14);
	tabWidget_->setLabelFont(font);

	tabWidget_->addTab("Content", libraryContent_);
	tabWidget_->addTab("Engine", libraryEngine_);
	tabWidget_->addTab("Extra", libraryExtra_);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 6, 0);
	layout->setSpacing(0);
	layout->addWidget(tabWidget_);
	setLayout(layout);
}

LibraryWidget::~LibraryWidget()
{
}

void LibraryWidget::addContent(const QString& id)
{
	libraryContent_->addContent(id);
	tabWidget_->addNotification(tabWidget_->indexOf(libraryContent_));
}

void LibraryWidget::removeContent(const QString& id)
{
	libraryContent_->removeContent(id);
}

void LibraryWidget::addProject(const QString& id)
{
	libraryContent_->addProject(id);
	tabWidget_->addNotification(tabWidget_->indexOf(libraryContent_));
}

void LibraryWidget::removeProject(const QString& id)
{
	libraryContent_->removeProject(id);
}

void LibraryWidget::addEngine(const QString& engineName, const QString& engineVersion)
{
	libraryEngine_->addEngine(engineName, engineVersion);
	tabWidget_->addNotification(tabWidget_->indexOf(libraryEngine_));
}

void LibraryWidget::removeEngine(const QString& engineName, const QString& engineVersion)
{
	libraryEngine_->removeEngine(engineName, engineVersion);
}

void LibraryWidget::addExtra(const QString& id)
{
	libraryExtra_->addExtra(id);
	tabWidget_->addNotification(tabWidget_->indexOf(libraryExtra_));
}

void LibraryWidget::removeExtra(const QString& id)
{
	libraryExtra_->removeExtra(id);
}

void LibraryWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

