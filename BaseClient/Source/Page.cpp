#include "Page.h"

#include <QPainter>

#include <string>

Page::Page(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	Rpc::StringSeq categories;
	context_->session->getCategories(categories);

	for (const std::string& category : categories) {
		ui_.categoryBox->addItem(category.c_str());
	}
}

Page::~Page()
{
}

void Page::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		firstShow_ = false;
	}
}

void Page::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
