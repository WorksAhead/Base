#include "ManageCategoryWidget.h"
#include "ErrorMessage.h"

#include "CategorySelectorDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>

#include <boost/algorithm/string.hpp>

ManageCategoryWidget::ManageCategoryWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageCategoryWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageCategoryWidget::onSubmit);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageCategoryWidget::onEdit);
}

ManageCategoryWidget::~ManageCategoryWidget()
{
}

void ManageCategoryWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageCategoryWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageCategoryWidget::onRefresh()
{
	Rpc::StringSeq categories;
	Rpc::ErrorCode ec = context_->session->getCategories(categories);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	ui_.categoryEdit->clear();

	std::string list;

	for (const std::string& cat : categories) {
		list += cat;
		list += "\n";
	}

	ui_.categoryEdit->setPlainText(list.c_str());
}

void ManageCategoryWidget::onSubmit()
{
	Rpc::StringSeq categories;

	std::istringstream stream(ui_.categoryEdit->toPlainText().toStdString());
	std::string line;
	while (std::getline(stream, line)) {
		categories.push_back(line);
	}

	Rpc::ErrorCode ec = context_->session->setCategories(categories);

	context_->promptRpcError(ec);
}

void ManageCategoryWidget::onEdit()
{
	//ui_.categoryEdit->setReadOnly(false);
	//ui_.categoryEdit->setFocus();

	Rpc::StringSeq categories;
	Rpc::ErrorCode ec = context_->session->getCategories(categories);

	QStringList list;

	for (const std::string& cat : categories) {
		list << cat.c_str();
	}

	CategorySelectorDialog d;
	d.categorySelectorWidget()->setCategories(list);
	d.exec();
}

