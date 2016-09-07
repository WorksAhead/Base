#include "HTabBarTabWidget.h"

#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>

HTabBarTabWidget::HTabBarTabWidget(QWidget* parent) : QWidget(parent)
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
	l2->setMargin(0);
	l2->setSpacing(0);
	l2->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	l2->addWidget(superscript_);

	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setMargin(0);
	layout_->setSpacing(0);
	layout_->addLayout(l2);
	layout_->addWidget(label_);

	setLayout(layout_);

	setProperty("active", false);
}

HTabBarTabWidget::~HTabBarTabWidget()
{

}

void HTabBarTabWidget::setLabel(const QString& text)
{
	label_->setText(text);
}

void HTabBarTabWidget::setLabelFont(const QFont& font)
{
	const int p = font.pixelSize();
	label_->setContentsMargins(p * 1.2, 0, p * 1.2, p * 0.8);
	label_->setFont(font);
}

void HTabBarTabWidget::setActive(bool active)
{
	style()->unpolish(this);
	setProperty("active", active);
	style()->polish(this);
}

void HTabBarTabWidget::addNotification()
{
	superscript_->setText(QString("%1").arg(superscript_->text().toInt() + 1));
	superscript_->setVisible(true);
}

void HTabBarTabWidget::clearNotification()
{
	superscript_->setText("0");
	superscript_->setVisible(false);
}

void HTabBarTabWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

