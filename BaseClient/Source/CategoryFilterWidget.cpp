#include "CategoryFilterWidget.h"

#include <QBoxLayout>
#include <QPushButton>

CategoryFilterWidget::CategoryFilterWidget(QWidget* parent) : QWidget(parent)
{
	collapseButton_ = new QPushButton;
	collapseButton_->setObjectName("CategoryFilterCollapse");
	collapseButton_->setFixedSize(10, 40);
	collapseButton_->setIcon(QIcon(":/Icons/CategoryFilterCollapse.png"));

	selector_ = new LabelSelectorWidget;

	QBoxLayout* l2 = new QBoxLayout(QBoxLayout::TopToBottom);

	l2->addWidget(selector_);
	l2->addStretch();

	QWidget* w = new QWidget;
	w->setObjectName("CategoryFilterWidget");
	w->setLayout(l2);

	area_ = new QScrollArea;
	area_->setObjectName("CategoryFilterArea");
	area_->setFixedWidth(260);
	area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	area_->setWidgetResizable(true);
	area_->setWidget(w);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(collapseButton_);
	layout->addWidget(area_);

	setLayout(layout);

	QObject::connect(collapseButton_, &QPushButton::clicked, [this]() {
		if (area_->isVisible()) {
			Q_EMIT collapsed();
		}
		else {
			Q_EMIT extended();
		}
	});
}

CategoryFilterWidget::~CategoryFilterWidget()
{
}

LabelSelectorWidget* CategoryFilterWidget::labelSelectorWidget()
{
	return selector_;
}

void CategoryFilterWidget::collapse()
{
	area_->setVisible(false);
	collapseButton_->setIcon(QIcon(":/Icons/CategoryFilterExtend.png"));
}

void CategoryFilterWidget::extend()
{
	area_->setVisible(true);
	collapseButton_->setIcon(QIcon(":/Icons/CategoryFilterCollapse.png"));
}

