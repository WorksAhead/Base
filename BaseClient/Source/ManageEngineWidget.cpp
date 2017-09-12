#include "ManageEngineWidget.h"
#include "SubmitEngineDialog.h"
#include "ASyncSubmitEngineTask.h"
#include "ErrorMessage.h"

#include <Crc.h>

#include <QPainter>
#include <QMessageBox>
#include <QInputDialog>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <numeric>
#include <memory>

#define ITEMS_PER_REQUEST 100

namespace fs = boost::filesystem;

ManageEngineWidget::ManageEngineWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.engineList->header()->setSortIndicator(4, Qt::DescendingOrder);

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageEngineWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageEngineWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageEngineWidget::onRefresh);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageEngineWidget::onEdit);
	QObject::connect(ui_.changeDisplayPriorityButton, &QPushButton::clicked, this, &ManageEngineWidget::onChangeDisplayPriority);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageEngineWidget::onRemove);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageEngineWidget::showSubmitDialog);

	if (context_->currentUserGroup == "Admin") {
		ui_.changeDisplayPriorityButton->setEnabled(true);
	}

	firstShow_ = true;
}

ManageEngineWidget::~ManageEngineWidget()
{
}

void ManageEngineWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageEngineWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageEngineWidget::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageEngineWidget::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageEngineWidget::onRefresh()
{
	ui_.engineList->clear();
	context_->session->browseEngineVersions(true, browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void ManageEngineWidget::onEdit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();
	if (items.count() != 1) {
		QMessageBox::information(0, "Base", tr("Please select a single Engine version."));
		return;
	}

	SubmitEngineDialog d;
	d.switchToEditMode();

	d.setEngineName(items[0]->text(0));
	d.setEngineVersion(items[0]->text(1));
	d.setSetup(items[0]->text(2));
	d.setUnSetup(items[0]->text(3));
	d.setInfo(items[0]->text(5));

	if (d.exec() == 1)
	{
		Rpc::EngineVersionSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->updateEngineVersion(d.getEngineName().toStdString(), d.getEngineVersion().toStdString(), submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getSetup().isEmpty()) {
			ec = submitter->setSetup(d.getSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getUnSetup().isEmpty()) {
			ec = submitter->setUnSetup(d.getUnSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		ec = submitter->finish();
		context_->promptRpcError(ec);
	}

#undef CHECK_ERROR_CODE
}

void ManageEngineWidget::onRemove()
{
	const int ret = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove these Engine versions ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (ret != QMessageBox::Yes) {
		return;
	}

	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		context_->session->removeEngineVersion(items[i]->text(0).toStdString(), items[i]->text(1).toStdString());
	}
}

void ManageEngineWidget::showSubmitDialog()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		context_->promptRpcError(ec);								\
		return;														\
	}

	SubmitEngineDialog d;

	if (d.exec() == 1)
	{
		Rpc::EngineVersionSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->submitEngineVersion(d.getEngineName().toStdString(), d.getEngineVersion().toStdString(), submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getSetup().isEmpty()) {
			ec = submitter->setSetup(d.getSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getUnSetup().isEmpty()) {
			ec = submitter->setUnSetup(d.getUnSetup().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getInfo().isEmpty()) {
			ec = submitter->setInfo(d.getInfo().toStdString());
			CHECK_ERROR_CODE(ec);
		}

		boost::shared_ptr<ASyncSubmitEngineTask> task(new ASyncSubmitEngineTask(context_, submitter));
		task->setInfoHead(QString("Submit %1 %2").arg(d.getEngineName(), d.getEngineVersion()).toStdString());
		task->setPath(d.getLocation().toLocal8Bit().data());

		context_->addTask(task);
	}

#undef CHECK_ERROR_CODE
}

void ManageEngineWidget::onChangeDisplayPriority()
{
	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();

	if (items.count() == 0) {
		return;
	}

	bool ok;

	int displayPriority = QInputDialog::getInt(this, "Base", "DisplayPriority", 0, -2147483647, 2147483647, 1, &ok, Qt::WindowTitleHint);

	if (!ok) {
		return;
	}

	for (int i = 0; i < items.count(); ++i)
	{
		context_->session->changeEngineVersionDisplayPriority(items[i]->text(0).toStdString(), items[i]->text(1).toStdString(), displayPriority);
	}
}

void ManageEngineWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::EngineVersionSeq engineItems;
		browser_->next(n, engineItems);

		QList<QTreeWidgetItem*> items;

		for (int i = 0; i < engineItems.size(); ++i)
		{
			QStringList list;
			list << engineItems[i].name.c_str();
			list << engineItems[i].version.c_str();
			list << engineItems[i].setup.c_str();
			list << engineItems[i].unsetup.c_str();
			list << engineItems[i].uptime.c_str();
			boost::replace_all(engineItems[i].info, "\n", "\r");
			list << engineItems[i].info.c_str();
			list << QString("%1").arg(engineItems[i].displayPriority);
			list << engineItems[i].state.c_str();
			QTreeWidgetItem* item = new QTreeWidgetItem(list);
			items.append(item);
		}

		ui_.engineList->addTopLevelItems(items);

		count -= n;

		if (engineItems.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

