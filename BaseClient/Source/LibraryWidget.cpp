#include "LibraryWidget.h"
#include "VTabWidget.h"

#include <QBoxLayout>
#include <QTextEdit>

LibraryWidget::LibraryWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	VTabWidget* p = new VTabWidget;

	p->addTab("Hello World", new QTextEdit);

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

