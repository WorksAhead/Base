#include "ManageInfoWidget.h"
#include "PagesEditDialog.h"
#include "CategoriesEditDialog.h"

#include <QMessageBox>
#include <QTextStream>
#include <QPainter>

ManageInfoWidget::ManageInfoWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageInfoWidget::onRefresh);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageInfoWidget::onEdit);

	firstShow_ = true;
}

ManageInfoWidget::~ManageInfoWidget()
{
}

void ManageInfoWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageInfoWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageInfoWidget::onRefresh()
{
	ui_.list->clear();

	Rpc::ErrorCode ec;

	{
		Rpc::StringSeq pages;

		if ((ec = context_->session->getPages(pages)) != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		QString formattedPages;

		for (const std::string& page : pages) {
			formattedPages.append(page.c_str());
			formattedPages.append("\r");
		}

		ui_.list->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Pages" << formattedPages));
	}

	{
		Rpc::StringSeq categories;

		if ((ec = context_->session->getContentCategories(categories)) != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		QString formattedCategories;

		for (const std::string& cat : categories) {
			formattedCategories.append(cat.c_str());
			formattedCategories.append("\r");
		}

		ui_.list->addTopLevelItem(new QTreeWidgetItem(QStringList() << "ContentCategories" << formattedCategories));
	}

	{
		Rpc::StringSeq categories;

		if ((ec = context_->session->getExtraCategories(categories)) != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		QString formattedCategories;

		for (const std::string& cat : categories) {
			formattedCategories.append(cat.c_str());
			formattedCategories.append("\r");
		}

		ui_.list->addTopLevelItem(new QTreeWidgetItem(QStringList() << "ExtraCategories" << formattedCategories));
	}
}

void ManageInfoWidget::onEdit()
{
	QList<QTreeWidgetItem*> items = ui_.list->selectedItems();

	if (items.count() == 0 || items.count() > 1) {
		QMessageBox::information(0, "Base", tr("Please select an Item."));
		return;
	}

	if (items[0]->text(0) == "Pages")
	{
		QString s = items[0]->text(1);
		s.replace('\r', '\n');

		PagesEditDialog d(s, this);

		if (d.exec())
		{
			Rpc::StringSeq pages;

			QString text = d.text();
			QTextStream stream(&text, QIODevice::ReadOnly);

			QString line;

			while (stream.readLineInto(&line)) {
				pages.push_back(line.trimmed().toStdString());
			}

			Rpc::ErrorCode ec = context_->session->setPages(pages);

			context_->promptRpcError(ec);
		}
	}
	else if (items[0]->text(0) == "ContentCategories")
	{
		QString s = items[0]->text(1);
		s.replace('\r', '\n');

		CategoriesEditDialog d(s, this);

		if (d.exec())
		{
			Rpc::StringSeq categories;

			QString text = d.text();
			QTextStream stream(&text, QIODevice::ReadOnly);

			QString line;

			while (stream.readLineInto(&line)) {
				categories.push_back(line.trimmed().toStdString());
			}

			Rpc::ErrorCode ec = context_->session->setContentCategories(categories);

			context_->promptRpcError(ec);
		}
	}
	else if (items[0]->text(0) == "ExtraCategories")
	{
		QString s = items[0]->text(1);
		s.replace('\r', '\n');

		CategoriesEditDialog d(s, this);

		if (d.exec())
		{
			Rpc::StringSeq categories;

			QString text = d.text();
			QTextStream stream(&text, QIODevice::ReadOnly);

			QString line;

			while (stream.readLineInto(&line)) {
				categories.push_back(line.trimmed().toStdString());
			}

			Rpc::ErrorCode ec = context_->session->setExtraCategories(categories);

			context_->promptRpcError(ec);
		}
	}
}

