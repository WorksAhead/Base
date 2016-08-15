#include "ManageContentWidget.h"
#include "ErrorMessage.h"
#include "SubmitContentDialog.h"

#include <QPainter>
#include <QMessageBox>
#include <QMenu>

#include <boost/algorithm/string.hpp>

#include <sstream>

#define ITEMS_PER_REQUEST 100

ManageContentWidget::ManageContentWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.contentList->header()->setSortIndicator(11, Qt::DescendingOrder);

	/*QMenu* menu = new QMenu;
	ui_.setGroupButton->setMenu(menu);

	QAction* setUserGroupAction = menu->addAction("User Group");
	QAction* setAdminGroupAction = menu->addAction("Admin Group");

	QObject::connect(setUserGroupAction, &QAction::triggered, [this](){
		QList<QTreeWidgetItem*> items = ui_.userList->selectedItems();
		for (int i = 0; i < items.count(); ++i) {
			Rpc::ErrorCode ec = context_->session->setUserGroup(items[i]->text(0).toStdString(), "User");
			if (ec == Rpc::ec_success) {
				items[i]->setText(1, "User");
			}
		}
	});

	QObject::connect(setAdminGroupAction, &QAction::triggered, [this](){
		QList<QTreeWidgetItem*> items = ui_.userList->selectedItems();
		for (int i = 0; i < items.count(); ++i) {
			Rpc::ErrorCode ec = context_->session->setUserGroup(items[i]->text(0).toStdString(), "Admin");
			if (ec == Rpc::ec_success) {
				items[i]->setText(1, "Admin");
			}
		}
	});*/

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageContentWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageContentWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageContentWidget::onRefresh);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageContentWidget::onEdit);
	QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageContentWidget::onRemove);

	firstShow_ = true;
}

ManageContentWidget::~ManageContentWidget()
{
}

void ManageContentWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageContentWidget::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageContentWidget::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageContentWidget::onRefresh()
{
	ui_.contentList->clear();
	context_->session->browseContent("", "", browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void ManageContentWidget::onEdit()
{
	QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();
	if (items.count() == 0 || items.count() > 1) {
		QMessageBox::information(0, "Base", tr("Please select a single Content."));
		return;
	}

	Rpc::ErrorCode ec;

	Rpc::ContentInfo ci;
	if ((ec = context_->session->getContentInfo(items[0]->text(0).toStdString(), ci)) != Rpc::ec_success) {
		context_->promptRpcError(ec);
		return;
	}

	std::string command;
	std::string workDir;
	std::istringstream stream(ci.startup);
	getline(stream, command);
	getline(stream, workDir);

	SubmitContentDialog d(context_, this);

	d.switchToEditMode(ci.id.c_str());

	d.setParentId(ci.parentId.c_str());
	d.setTitle(ci.title.c_str());
	d.setPage(ci.page.c_str());
	d.setCategory(ci.category.c_str());
	d.setEngineName(ci.engineName.c_str());
	d.setEngineVersion(ci.engineVersion.c_str());
	d.setCommand(command.c_str());
	d.setWorkingDir(workDir.c_str());
	d.setDesc(ci.desc.c_str());

	d.exec();
}

void ManageContentWidget::onRemove()
{
	const int ret = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove these Contents ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (ret != QMessageBox::Yes) {
		return;
	}

	int count = 0;

	QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		Rpc::ErrorCode ec = context_->session->removeContent(items[i]->text(0).toStdString());
		if (ec == Rpc::ec_success) {
			++count;
			delete items[i];
		}
	}

	QMessageBox::information(0, "Base",
		QString(tr("%1 Contents have been removed.")).arg(count),
		QMessageBox::Yes);
}

void ManageContentWidget::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::ContentItemSeq items;
		browser_->next(n, items);

		QList<QTreeWidgetItem*> widgetItems;

		for (int i = 0; i < items.size(); ++i)
		{
			Rpc::ContentInfo ci;
			if (context_->session->getContentInfo(items[i].id, ci) != Rpc::ec_success) {
				continue;
			}

			if (!boost::iequals(context_->currentUser, ci.user) && context_->currentUserGroup != "Admin") {
				continue;
			}

			std::string command;
			std::string workDir;
			std::istringstream stream(ci.startup);
			getline(stream, command);
			getline(stream, workDir);

			boost::replace_all(ci.desc, "\n", "\r");

			QStringList list;
			list << ci.id.c_str();
			list << ci.parentId.c_str();
			list << ci.title.c_str();
			list << ci.page.c_str();
			list << ci.category.c_str();
			list << ci.engineName.c_str();
			list << ci.engineVersion.c_str();
			list << command.c_str();
			list << workDir.c_str();
			list << ci.desc.c_str();
			list << ci.user.c_str();
			list << ci.upTime.c_str();
			list << ci.state.c_str();

			QTreeWidgetItem* wi = new QTreeWidgetItem(list);
			widgetItems.append(wi);
		}

		ui_.contentList->addTopLevelItems(widgetItems);

		count -= n;

		if (items.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

