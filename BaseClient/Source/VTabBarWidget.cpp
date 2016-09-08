#include "VTabBarWidget.h"
#include "VTabBarTabWidget.h"

#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

VTabBarWidget::VTabBarWidget(QWidget* parent) : QWidget(parent)
{
	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	layout_->setMargin(0);
	layout_->setSpacing(0);

	setLayout(layout_);
}

VTabBarWidget::~VTabBarWidget()
{

}

void VTabBarWidget::addTab(const QString& label)
{
	insertTab(-1, label);
}

void VTabBarWidget::insertTab(int index, const QString& label)
{
	if (index < 0 || index > layout_->count()) {
		index = layout_->count();
	}

	VTabBarTabWidget* tab = new VTabBarTabWidget;
	tab->setLabelFont(labelFont_);
	tab->setLabel(label);

	layout_->insertWidget(index, tab);

	if (index == 0) {
		setCurrentIndex(index);
	}
}

void VTabBarWidget::addNotification(int index)
{
	VTabBarTabWidget* tab = (VTabBarTabWidget*)layout_->itemAt(index)->widget();
	tab->addNotification();
}

void VTabBarWidget::setCurrentIndex(int index)
{
	for (int i = 0; i < layout_->count(); ++i) {
		VTabBarTabWidget* tab = (VTabBarTabWidget*)layout_->itemAt(i)->widget();
		if (i == index) {
			tab->setActive(true);
		}
		else {
			tab->setActive(false);
		}
	}
}

QFont VTabBarWidget::labelFont() const
{
	return labelFont_;
}

void VTabBarWidget::setLabelFont(const QFont& font)
{
	labelFont_ = font;

	for (int i = 0; i < layout_->count(); ++i) {
		VTabBarTabWidget* tab = (VTabBarTabWidget*)layout_->itemAt(i)->widget();
		tab->setLabelFont(font);
	}
}

void VTabBarWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void VTabBarWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		VTabBarTabWidget* tab = tabAt(e->pos());
		if (tab) {
			tab->clearNotification();
			int index = layout_->indexOf(tab);
			setCurrentIndex(index);
			Q_EMIT currentIndexChanged(index);
		}
	}
}

void VTabBarWidget::mouseReleaseEvent(QMouseEvent*)
{

}

void VTabBarWidget::mouseDoubleClickEvent(QMouseEvent*)
{

}

void VTabBarWidget::mouseMoveEvent(QMouseEvent* e)
{
}

VTabBarTabWidget* VTabBarWidget::tabAt(const QPoint& pos)
{
	QWidget* w = childAt(pos);

	while (w)
	{
		VTabBarTabWidget* tab = qobject_cast<VTabBarTabWidget*>(w);
		if (tab) {
			return tab;
		}
		w = w->parentWidget();
	}

	return 0;
}
