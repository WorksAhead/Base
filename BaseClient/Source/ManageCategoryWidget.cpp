#include "ManageCategoryWidget.h"
#include "ErrorMessage.h"

#include "LabelSelectorDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QTextStream>

#include <boost/algorithm/string.hpp>

#include <string>

ManageCategoryWidget::ManageCategoryWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageCategoryWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageCategoryWidget::onSubmit);
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

	QString text = ui_.categoryEdit->toPlainText();
	QTextStream stream(&text, QIODevice::ReadOnly);

	QString line;
	qint64 lastPos = 0;

	while (stream.readLineInto(&line))
	{
		int pos = line.indexOf(QRegularExpression("[,()]"));
		if (pos >= 0) {
			pos = lastPos + pos;
			QTextCursor c = ui_.categoryEdit->textCursor();
			c.setPosition(pos);
			c.setPosition(pos + 1, QTextCursor::KeepAnchor);
			ui_.categoryEdit->setTextCursor(c);
			ui_.categoryEdit->setFocus();
			return;
		}

		lastPos = stream.pos();
		categories.push_back(line.trimmed().toStdString());
	}

	Rpc::ErrorCode ec = context_->session->setCategories(categories);

	context_->promptRpcError(ec);
}
