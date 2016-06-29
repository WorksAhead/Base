#include "ManagePage.h"
#include "ErrorMessage.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>

ManagePage::ManagePage(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManagePage::onRefresh);
	QObject::connect(ui_.moveUpButton, &QPushButton::clicked, this, &ManagePage::onMoveUp);
	QObject::connect(ui_.moveDownButton, &QPushButton::clicked, this, &ManagePage::onMoveDown);
	QObject::connect(ui_.addButton, &QPushButton::clicked, this, &ManagePage::onAdd);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManagePage::onRemove);
	QObject::connect(ui_.saveButton, &QPushButton::clicked, this, &ManagePage::onSave);
}

ManagePage::~ManagePage()
{
}

void ManagePage::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManagePage::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManagePage::onRefresh()
{
	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = context_->session->getPages(pages);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	ui_.pageList->clear();

	for (const std::string& page : pages) {
		ui_.pageList->addItem(page.c_str());
	}
}

void ManagePage::onMoveUp()
{
	int row = ui_.pageList->currentRow();
	if (row > 0) {
		QListWidgetItem* li = ui_.pageList->takeItem(row);
		ui_.pageList->insertItem(row - 1, li);
		ui_.pageList->setCurrentRow(row - 1);
	}
}

void ManagePage::onMoveDown()
{
	int row = ui_.pageList->currentRow();
	if (row < ui_.pageList->count() - 1) {
		QListWidgetItem* li = ui_.pageList->takeItem(row);
		ui_.pageList->insertItem(row + 1, li);
		ui_.pageList->setCurrentRow(row + 1);
	}
}

void ManagePage::onAdd()
{
	bool ok;
	QString text = QInputDialog::getText(this, "Add Page", "Page:", QLineEdit::Normal, QString(), &ok);
	if (!ok) {
		return;
	}

	text = text.trimmed();

	if (text.isEmpty() || text.contains(',')) {
		QMessageBox::information(this, "Base", tr("Invalid page name"));
		return;
	}

	ui_.pageList->addItem(text);
}

void ManagePage::onRemove()
{
	delete ui_.pageList->currentItem();
}


void ManagePage::onSave()
{
	Rpc::StringSeq pages;

	for (int i = 0; i < ui_.pageList->count(); ++i) {
		pages.push_back(ui_.pageList->item(i)->text().toStdString());
	}

	Rpc::ErrorCode ec = context_->session->setPages(pages);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}
}

