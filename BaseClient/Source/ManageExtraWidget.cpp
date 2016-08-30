#include "ManageExtraWidget.h"
#include "ErrorMessage.h"
#include "SubmitExtraDialog.h"
#include "ASyncSubmitExtraTask.h"

#include <QPainter>
#include <QMessageBox>
#include <QMenu>

#include <boost/algorithm/string.hpp>

#include <sstream>

#define ITEMS_PER_REQUEST 100

ManageExtraWidget::ManageExtraWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.extraList->header()->setSortIndicator(4, Qt::DescendingOrder);

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageExtraWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageExtraWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageExtraWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageExtraWidget::onSubmit);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageExtraWidget::onEdit);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageExtraWidget::onRemove);

	firstShow_ = true;
}

ManageExtraWidget::~ManageExtraWidget()
{
}

void ManageExtraWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageExtraWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageExtraWidget::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageExtraWidget::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageExtraWidget::onRefresh()
{
	ui_.extraList->clear();
	context_->session->browseExtra(browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void ManageExtraWidget::onSubmit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	SubmitExtraDialog d;

	if (d.exec() == 1)
	{
		Rpc::ExtraSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->submitExtra(submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getTitle().isEmpty()) {
			ec = submitter->setTitle(d.getTitle().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getSetup().isEmpty()) {
			ec = submitter->setSetup(d.getSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		boost::shared_ptr<ASyncSubmitExtraTask> task(new ASyncSubmitExtraTask(context_, submitter));
		task->setInfoHead(QString("Submit %1").arg(d.getTitle()).toLocal8Bit().data());
		task->setPath(d.getLocation().toLocal8Bit().data());

		context_->addTask(task);
	}

#undef CHECK_ERROR_CODE
}

void ManageExtraWidget::onEdit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	QList<QTreeWidgetItem*> items = ui_.extraList->selectedItems();
	if (items.count() == 0 || items.count() > 1) {
		QMessageBox::information(0, "Base", tr("Please select a single Extra."));
		return;
	}

	Rpc::ErrorCode ec;

	Rpc::ExtraInfo ei;
	if ((ec = context_->session->getExtraInfo(items[0]->text(0).toStdString(), ei)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	SubmitExtraDialog d;

	d.switchToEditMode();
	
	d.setTitle(ei.title.c_str());
	d.setSetup(ei.setup.c_str());
	d.setInfo(ei.info.c_str());

	if (d.exec() == 1)
	{
		Rpc::ExtraSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->updateExtra(ei.id, submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getTitle().isEmpty()) {
			ec = submitter->setTitle(d.getTitle().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getSetup().isEmpty()) {
			ec = submitter->setSetup(d.getSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		ec = submitter->finish();
		CHECK_ERROR_CODE(ec);
	}

#undef CHECK_ERROR_CODE
}

void ManageExtraWidget::onRemove()
{
	const int ret = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove these Extras ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (ret != QMessageBox::Yes) {
		return;
	}

	int count = 0;

	QList<QTreeWidgetItem*> items = ui_.extraList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		Rpc::ErrorCode ec = context_->session->removeExtra(items[i]->text(0).toStdString());
		if (ec == Rpc::ec_success) {
			++count;
			delete items[i];
		}
	}

	QMessageBox::information(0, "Base",
		QString(tr("%1 Contents have been removed.")).arg(count),
		QMessageBox::Yes);
}

void ManageExtraWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::ExtraInfoSeq items;
		browser_->next(n, items);

		QList<QTreeWidgetItem*> widgetItems;

		for (int i = 0; i < items.size(); ++i)
		{
			Rpc::ExtraInfo& item = items[i];

			if (!boost::iequals(context_->currentUser, item.user) && context_->currentUserGroup != "Admin") {
				continue;
			}

			boost::replace_all(item.info, "\n", "\r");

			QStringList list;
			list << item.id.c_str();
			list << item.title.c_str();
			list << item.setup.c_str();
			list << item.user.c_str();
			list << item.uptime.c_str();
			list << item.info.c_str();
			list << item.state.c_str();

			QTreeWidgetItem* wi = new QTreeWidgetItem(list);
			widgetItems.append(wi);
		}

		ui_.extraList->addTopLevelItems(widgetItems);

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

