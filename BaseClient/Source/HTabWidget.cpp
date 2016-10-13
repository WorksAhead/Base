#include "HTabWidget.h"

HTabWidget::HTabWidget(QWidget* parent) : QWidget(parent)
{
	tabBar_ = new HTabBarWidget;
	stack_ = new QStackedLayout;

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(tabBar_);
	layout->addLayout(stack_);

	setLayout(layout);

	QObject::connect(tabBar_, &HTabBarWidget::currentIndexChanged, stack_, &QStackedLayout::setCurrentIndex);
}

HTabWidget::~HTabWidget()
{

}

HTabBarWidget* HTabWidget::tabBar()
{
	return tabBar_;
}

int HTabWidget::count() const
{
	return stack_->count();
}

QWidget* HTabWidget::widget(int index) const
{
	return stack_->widget(index);
}

void HTabWidget::addTab(const QString& label, QWidget* content)
{
	insertTab(-1, label, content);
}

void HTabWidget::insertTab(int index, const QString& label, QWidget* content)
{
	if (index < 0 || index > stack_->count()) {
		index = stack_->count();
	}

	tabBar_->insertTab(index, label);
	stack_->insertWidget(index, content);
}

int HTabWidget::indexOf(QWidget* content)
{
	return stack_->indexOf(content);
}

void HTabWidget::addNotification(int index)
{
	tabBar_->addNotification(index);
}

int HTabWidget::currentIndex() const
{
	return stack_->currentIndex();
}

void HTabWidget::setCurrentIndex(int index)
{
	tabBar_->setCurrentIndex(index);
	stack_->setCurrentIndex(index);
}

QFont HTabWidget::labelFont() const
{
	return tabBar_->labelFont();
}

void HTabWidget::setLabelFont(const QFont& font)
{
	tabBar_->setLabelFont(font);
}

