#include "ManageClientWidget.h"
#include "SubmitClientDialog.h"
#include "ASyncSubmitClientTask.h"
#include "ErrorMessage.h"

#include <QPainter>
#include <QMessageBox>
#include <QMenu>

#include <boost/algorithm/string.hpp>

#include <sstream>

#define ITEMS_PER_REQUEST 100

ManageClientWidget::ManageClientWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.clientList->header()->setSortIndicator(1, Qt::DescendingOrder);

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageClientWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageClientWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageClientWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageClientWidget::onSubmit);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageClientWidget::onEdit);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageClientWidget::onRemove);

	firstShow_ = true;
}

ManageClientWidget::~ManageClientWidget()
{
}

void ManageClientWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageClientWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageClientWidget::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageClientWidget::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageClientWidget::onRefresh()
{
	ui_.clientList->clear();
	context_->session->browseClient(browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void ManageClientWidget::onSubmit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	SubmitClientDialog d(context_, this);

	if (d.exec() == 1)
	{
		Rpc::ClientSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->submitClient(d.getVersion().toStdString(), submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		boost::shared_ptr<ASyncSubmitClientTask> task(new ASyncSubmitClientTask(context_, submitter));
		task->setInfoHead(QString("Submit Client %1").arg(d.getVersion()).toStdString());
		task->setPath(d.getLocation().toLocal8Bit().data());

		context_->addTask(task);
	}

#undef CHECK_ERROR_CODE
}

void ManageClientWidget::onEdit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	QList<QTreeWidgetItem*> items = ui_.clientList->selectedItems();
	if (items.count() == 0 || items.count() > 1) {
		QMessageBox::information(0, "Base", tr("Please select a single Client."));
		return;
	}

	Rpc::ErrorCode ec;

	Rpc::ClientInfo ci;
	if ((ec = context_->session->getClientInfo(items[0]->text(0).toStdString(), ci)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
	}

	SubmitClientDialog d(context_, this);

	d.switchToEditMode();

	d.setVersion(ci.version.c_str());
	d.setInfo(ci.info.c_str());

	if (d.exec() == 1)
	{
		Rpc::ClientSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->updateClient(d.getVersion().toStdString(), submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		ec = submitter->finish();
		context_->promptRpcError(ec);
	}

#undef CHECK_ERROR_CODE
}

void ManageClientWidget::onRemove()
{
	const int ret = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove these Client versions ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (ret != QMessageBox::Yes) {
		return;
	}

	QList<QTreeWidgetItem*> items = ui_.clientList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		context_->session->removeClient(items[i]->text(0).toStdString());
	}
}

void ManageClientWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::ClientInfoSeq items;
		browser_->next(n, items);

		QList<QTreeWidgetItem*> widgetItems;

		for (int i = 0; i < items.size(); ++i)
		{
			boost::replace_all(items[i].info, "\n", "\r");

			QStringList list;
			list << items[i].version.c_str();
			list << items[i].uptime.c_str();
			list << items[i].info.c_str();
			list << items[i].state.c_str();

			QTreeWidgetItem* wi = new QTreeWidgetItem(list);
			widgetItems.append(wi);
		}

		ui_.clientList->addTopLevelItems(widgetItems);

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

