#include "ManageExtraWidget.h"
#include "ErrorMessage.h"
#include "SubmitExtraDialog.h"
#include "ASyncSubmitExtraTask.h"

#include <QPainter>
#include <QMessageBox>
#include <QMenu>
#include <QPixmap>

#include <boost/algorithm/string.hpp>

#include <sstream>

#define ITEMS_PER_REQUEST 100

ManageExtraWidget::ManageExtraWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.extraList->header()->setSortIndicator(6, Qt::DescendingOrder);

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
	context_->session->browseExtra("", "", browser_);
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

	bool copyForm = false;
	bool fillParentId = false;

	QList<QTreeWidgetItem*> items = ui_.extraList->selectedItems();

	if (items.count() == 1)
	{
		QMessageBox msgBox(this);

		msgBox.setWindowTitle("Base");
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setText(tr("What do you want ?"));

		QPushButton* b0 = msgBox.addButton("A new form", QMessageBox::NoRole);
		QPushButton* b1 = msgBox.addButton("A copy of the selected form", QMessageBox::NoRole);
		QPushButton* b2 = msgBox.addButton("A sub-copy of the selected form", QMessageBox::NoRole);
		QPushButton* b3 = msgBox.addButton("Cancel", QMessageBox::NoRole);

		msgBox.exec();

		if (msgBox.clickedButton() == b3) {
			return;
		}
		else if (msgBox.clickedButton() == b1) {
			copyForm = true;
		}
		else if (msgBox.clickedButton() == b2) {
			copyForm = true;
			fillParentId = true;
		}
	}

	SubmitExtraDialog d(context_, this);

	if (copyForm)
	{
		Rpc::ExtraInfo ei;
		Rpc::ErrorCode ec;

		if ((ec = context_->session->getExtraInfo(items[0]->text(0).toStdString(), ei)) != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		d.loadCoverImageFrom(ei.id.c_str());

		if (fillParentId) {
			d.setParentId(ei.id.c_str());
		}
		else {
			d.setParentId(ei.parentId.c_str());
		}
		
		d.setTitle(ei.title.c_str());
		d.setCategory(ei.category.c_str());
		d.setSetup(ei.setup.c_str());
		d.setInfo(ei.info.c_str());
	}

	if (d.exec() == 1)
	{
		Rpc::ExtraSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->submitExtra(submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getParentId().isEmpty()) {
			ec = submitter->setParentId(d.getParentId().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getTitle().isEmpty()) {
			ec = submitter->setTitle(d.getTitle().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getCategory().isEmpty()) {
			ec = submitter->setCategory(d.getCategory().toStdString());
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
		task->setExtraLocation(d.getLocation().toLocal8Bit().data());

		if (!d.getCoverImage().isEmpty())
		{
			task->setImageFile(d.getCoverImage().toStdString());
		}

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

	SubmitExtraDialog d(context_, this);

	d.switchToEditMode();

	d.setParentId(ei.parentId.c_str());
	d.setTitle(ei.title.c_str());
	d.setCategory(ei.category.c_str());
	d.setSetup(ei.setup.c_str());
	d.setInfo(ei.info.c_str());

	if (d.exec() == 1)
	{
		Rpc::ExtraSubmitterPrx submitter;

		Rpc::ErrorCode ec = context_->session->updateExtra(ei.id, submitter);
		CHECK_ERROR_CODE(ec);

		if (!d.getParentId().isEmpty()) {
			ec = submitter->setParentId(d.getParentId().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getTitle().isEmpty()) {
			ec = submitter->setTitle(d.getTitle().toStdString());
			CHECK_ERROR_CODE(ec);
		}
		if (!d.getCategory().isEmpty()) {
			ec = submitter->setCategory(d.getCategory().toStdString());
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
		context_->promptRpcError(ec);
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

			std::string::size_type atPos = item.user.find('@');
			if (atPos != std::string::npos) {
				item.user.erase(atPos);
			}

			if (!boost::iequals(context_->currentUser, item.user) && context_->currentUserGroup != "Admin") {
				continue;
			}

			boost::replace_all(item.info, "\n", "\r");

			QStringList list;
			list << item.id.c_str();
			list << item.parentId.c_str();
			list << item.title.c_str();
			list << item.category.c_str();
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

