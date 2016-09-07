#include "HTabBarWidget.h"
#include "HTabBarTabWidget.h"

#include <QMouseEvent>

HTabBarWidget::HTabBarWidget(QWidget* parent) : QWidget(parent)
{
	layout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	layout_->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	layout_->setMargin(0);
	layout_->setSpacing(0);

	setLayout(layout_);
}

HTabBarWidget::~HTabBarWidget()
{

}

void HTabBarWidget::addTab(const QString& label)
{
	insertTab(-1, label);
}

void HTabBarWidget::insertTab(int index, const QString& label)
{
	if (index < 0 || index > layout_->count()) {
		index = layout_->count();
	}

	HTabBarTabWidget* tab = new HTabBarTabWidget;
	tab->setLabelFont(labelFont_);
	tab->setLabel(label);

	layout_->insertWidget(index, tab);

	if (index == 0) {
		setCurrentIndex(index);
	}
}

void HTabBarWidget::addNotification(int index)
{
	HTabBarTabWidget* tab = (HTabBarTabWidget*)layout_->itemAt(index)->widget();
	tab->addNotification();
}

void HTabBarWidget::setCurrentIndex(int index)
{
	for (int i = 0; i < layout_->count(); ++i) {
		HTabBarTabWidget* tab = (HTabBarTabWidget*)layout_->itemAt(i)->widget();
		if (i == index) {
			tab->setActive(true);
		}
		else {
			tab->setActive(false);
		}
	}
}

QFont HTabBarWidget::labelFont() const
{
	return labelFont_;
}

void HTabBarWidget::setLabelFont(const QFont& font)
{
	labelFont_ = font;

	for (int i = 0; i < layout_->count(); ++i) {
		HTabBarTabWidget* tab = (HTabBarTabWidget*)layout_->itemAt(i)->widget();
		tab->setLabelFont(font);
	}
}

void HTabBarWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		HTabBarTabWidget* tab = tabAt(e->pos());
		if (tab) {
			tab->clearNotification();
			int index = layout_->indexOf(tab);
			setCurrentIndex(index);
			Q_EMIT currentIndexChanged(index);
		}
	}
}

void HTabBarWidget::mouseReleaseEvent(QMouseEvent*)
{

}

void HTabBarWidget::mouseDoubleClickEvent(QMouseEvent*)
{

}

void HTabBarWidget::mouseMoveEvent(QMouseEvent* e)
{
}

HTabBarTabWidget* HTabBarWidget::tabAt(const QPoint& pos)
{
	QWidget* w = childAt(pos);

	while (w)
	{
		HTabBarTabWidget* tab = qobject_cast<HTabBarTabWidget*>(w);
		if (tab) {
			return tab;
		}
		w = w->parentWidget();
	}

	return 0;
}
