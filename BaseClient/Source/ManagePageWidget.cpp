#include "ManagePageWidget.h"
#include "ErrorMessage.h"

#include <QMessageBox>
#include <QPainter>
#include <QTextStream>

#include <boost/algorithm/string.hpp>

#include <string>

ManagePageWidget::ManagePageWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManagePageWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManagePageWidget::onSubmit);
}

ManagePageWidget::~ManagePageWidget()
{
}

void ManagePageWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManagePageWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManagePageWidget::onRefresh()
{
	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = context_->session->getPages(pages);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	ui_.pageEdit->clear();

	std::string list;

	for (const std::string& page : pages) {
		list += page;
		list += "\n";
	}

	ui_.pageEdit->setPlainText(list.c_str());
}

void ManagePageWidget::onSubmit()
{
	Rpc::StringSeq pages;

	QString text = ui_.pageEdit->toPlainText();
	QTextStream stream(&text, QIODevice::ReadOnly);

	QString line;
	//qint64 lastPos = 0;

	while (stream.readLineInto(&line))
	{
		//int pos = line.indexOf(QRegularExpression("[,()]"));
		//if (pos >= 0) {
		//	pos = lastPos + pos;
		//	QTextCursor c = ui_.pageEdit->textCursor();
		//	c.setPosition(pos);
		//	c.setPosition(pos + 1, QTextCursor::KeepAnchor);
		//	ui_.pageEdit->setTextCursor(c);
		//	ui_.pageEdit->setFocus();
		//	return;
		//}

		//lastPos = stream.pos();
		pages.push_back(line.trimmed().toStdString());
	}

	Rpc::ErrorCode ec = context_->session->setPages(pages);

	context_->promptRpcError(ec);
}

