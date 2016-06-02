#include "ManageEngine.h"
#include "SubmitEngineDialog.h"
#include "UploadTask.h"
#include "ErrorMessage.h"

#include <Crc.h>

#include <QPainter>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <numeric>

#define ITEMS_PER_REQUEST 100

namespace fs = boost::filesystem;

ManageEngine::ManageEngine(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);

	firstShow_ = true;

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageEngine::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageEngine::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageEngine::onRefresh);

	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageEngine::onRemove);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageEngine::showSubmitDialog);
}

ManageEngine::~ManageEngine()
{
}

void ManageEngine::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		context_->session->browseEngineVersions(browser_);
		showMore(ITEMS_PER_REQUEST);
		firstShow_ = false;
	}
}

void ManageEngine::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageEngine::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageEngine::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageEngine::onRefresh()
{
	ui_.engineList->clear();
	context_->session->browseEngineVersions(browser_);
	showMore(ITEMS_PER_REQUEST);
}

void ManageEngine::onRemove()
{
	const int rc = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove these versions ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (rc != QMessageBox::Yes) {
		return;
	}

	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		context_->session->removeEngineVersion(items[i]->text(0).toStdString(), items[i]->text(1).toStdString());
	}
}

void ManageEngine::showSubmitDialog()
{
	SubmitEngineDialog d;

	if (d.exec() == 1)
	{
		Rpc::UploaderPrx uploader;

		Rpc::ErrorCode ec = context_->session->uploadEngineVersion(d.engine().toStdString(), d.version().toStdString(), d.info().toStdString(), uploader);
		if (ec != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		context_->addTask(new UploadTask(context_, QString("Upload %1 %2").arg(d.engine(), d.version()).toStdString(), d.path().toStdString(), uploader));
	}
}

void ManageEngine::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::EngineVersionItemSeq engineItems;
		browser_->next(n, engineItems);

		QList<QTreeWidgetItem*> items;

		for (int i = 0; i < engineItems.size(); ++i)
		{
			QStringList list;
			list << engineItems[i].name.c_str();
			list << engineItems[i].version.c_str();
			list << engineItems[i].uptime.c_str();
			boost::replace_all(engineItems[i].info, "\n", "\r");
			list << engineItems[i].info.c_str();
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

