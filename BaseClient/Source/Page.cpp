#include "Page.h"
#include "SubmitContentDialog.h"

#include <QPainter>

Page::Page(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	Rpc::StringSeq categories;
	context_->session->getCategories(categories);

	for (const std::string& category : categories) {
		ui_.categoryBox->addItem(category.c_str());
	}

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &Page::foo);

	firstShow_ = true;
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

void Page::foo()
{
	SubmitContentDialog d(context_, this);
	d.setPage(name_);

	int rc = d.exec();

	if (rc == QDialog::Accepted)
	{
	}
}

