#include "VTabWidget.h"

#include <QBoxLayout>

VTabWidget::VTabWidget(QWidget* parent) : QWidget(parent)
{
	list_ = new QListWidget;
	list_->setObjectName("VTabList");

	list_->setFixedWidth(200);

	stack_ = new QStackedLayout;

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(list_);
	layout->addLayout(stack_);

	setLayout(layout);

	QObject::connect(list_, &QListWidget::currentRowChanged, stack_, &QStackedLayout::setCurrentIndex);
}

VTabWidget::~VTabWidget()
{
}

void VTabWidget::addTab(const QString& label, QWidget* content)
{
	insertTab(-1, label, content);
}

void VTabWidget::insertTab(int index, const QString& label, QWidget* content)
{
	if (index < 0 || index > stack_->count()) {
		index = stack_->count();
	}

	list_->insertItem(index, label);
	stack_->insertWidget(index, content);
}

void VTabWidget::setCurrentIndex(int index)
{
	list_->setCurrentRow(index);
	stack_->setCurrentIndex(index);
}

