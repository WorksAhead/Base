#include "VTabBarTabWidget.h"

#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>

VTabBarTabWidget::VTabBarTabWidget(QWidget* parent) : QWidget(parent)
{
	label_ = new QLabel;

	superscript_ = new QLabel(this);
	QSizePolicy sp = superscript_->sizePolicy();
	sp.setRetainSizeWhenHidden(true);
	superscript_->setSizePolicy(sp);
	superscript_->setObjectName("Notification");
	superscript_->setVisible(false);
	superscript_->setText("0");

	QBoxLayout* l2 = new QBoxLayout(QBoxLayout::LeftToRight);
	l2->setMargin(5);
	l2->setSpacing(0);
	l2->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	l2->addWidget(superscript_);

	layout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	layout_->setMargin(0);
	layout_->setSpacing(0);
	layout_->addWidget(label_);
	layout_->addLayout(l2);

	setLayout(layout_);

	setProperty("active", false);
}

VTabBarTabWidget::~VTabBarTabWidget()
{

}

void VTabBarTabWidget::setLabel(const QString& text)
{
	label_->setText(text);
}

void VTabBarTabWidget::setLabelFont(const QFont& font)
{
	const int p = font.pixelSize();
	label_->setContentsMargins(p * 1.2, p * 0.8, p * 1.2, p * 0.8);
	label_->setFont(font);
}

void VTabBarTabWidget::setActive(bool active)
{
	style()->unpolish(this);
	setProperty("active", active);
	style()->polish(this);
}

void VTabBarTabWidget::addNotification()
{
	superscript_->setText(QString("%1").arg(superscript_->text().toInt() + 1));
	superscript_->setVisible(true);
}

void VTabBarTabWidget::clearNotification()
{
	superscript_->setText("0");
	superscript_->setVisible(false);
}

void VTabBarTabWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

