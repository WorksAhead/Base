#include "VTabWidget.h"

VTabWidget::VTabWidget(QWidget* parent) : QWidget(parent)
{
	tabBar_ = new VTabBarWidget;
	stack_ = new QStackedLayout;

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(tabBar_);
	layout->addLayout(stack_);

	setLayout(layout);

	QObject::connect(tabBar_, &VTabBarWidget::currentIndexChanged, stack_, &QStackedLayout::setCurrentIndex);
}

VTabWidget::~VTabWidget()
{

}

VTabBarWidget* VTabWidget::tabBar()
{
	return tabBar_;
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

	tabBar_->insertTab(index, label);
	stack_->insertWidget(index, content);
}

int VTabWidget::indexOf(QWidget* content)
{
	return stack_->indexOf(content);
}

void VTabWidget::addNotification(int index)
{
	tabBar_->addNotification(index);
}

int VTabWidget::currentIndex() const
{
	return stack_->currentIndex();
}

void VTabWidget::setCurrentIndex(int index)
{
	tabBar_->setCurrentIndex(index);
	stack_->setCurrentIndex(index);
}

QFont VTabWidget::labelFont() const
{
	return tabBar_->labelFont();
}

void VTabWidget::setLabelFont(const QFont& font)
{
	tabBar_->setLabelFont(font);
}

