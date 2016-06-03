#include "ManageCategory.h"
#include "ErrorMessage.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>

ManageCategory::ManageCategory(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageCategory::onRefresh);
	QObject::connect(ui_.moveUpButton, &QPushButton::clicked, this, &ManageCategory::onMoveUp);
	QObject::connect(ui_.moveDownButton, &QPushButton::clicked, this, &ManageCategory::onMoveDown);
	QObject::connect(ui_.addButton, &QPushButton::clicked, this, &ManageCategory::onAdd);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageCategory::onRemove);
	QObject::connect(ui_.saveButton, &QPushButton::clicked, this, &ManageCategory::onSave);
}

ManageCategory::~ManageCategory()
{
}

void ManageCategory::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageCategory::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageCategory::onRefresh()
{
	Rpc::StringSeq categories;
	Rpc::ErrorCode ec = context_->session->getCategories(categories);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	ui_.categoryList->clear();

	for (const std::string& page : categories) {
		ui_.categoryList->addItem(page.c_str());
	}
}

void ManageCategory::onMoveUp()
{
	int row = ui_.categoryList->currentRow();
	if (row > 0) {
		QListWidgetItem* li = ui_.categoryList->takeItem(row);
		ui_.categoryList->insertItem(row - 1, li);
		ui_.categoryList->setCurrentRow(row - 1);
	}
}

void ManageCategory::onMoveDown()
{
	int row = ui_.categoryList->currentRow();
	if (row < ui_.categoryList->count() - 1) {
		QListWidgetItem* li = ui_.categoryList->takeItem(row);
		ui_.categoryList->insertItem(row + 1, li);
		ui_.categoryList->setCurrentRow(row + 1);
	}
}

void ManageCategory::onAdd()
{
	bool ok;
	QString text = QInputDialog::getText(this, "Add Category", "Category:", QLineEdit::Normal, QString(), &ok);
	if (!ok) {
		return;
	}

	text = text.trimmed();

	if (text.isEmpty() || text.contains(',')) {
		QMessageBox msg;
		msg.setWindowTitle("Base");
		msg.setText("Invalid category name");
		msg.exec();
		return;
	}

	ui_.categoryList->addItem(text);
}

void ManageCategory::onRemove()
{
	delete ui_.categoryList->currentItem();
}


void ManageCategory::onSave()
{
	Rpc::StringSeq categories;

	for (int i = 0; i < ui_.categoryList->count(); ++i) {
		categories.push_back(ui_.categoryList->item(i)->text().toStdString());
	}

	Rpc::ErrorCode ec = context_->session->setCategories(categories);
	if (ec != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}
}

