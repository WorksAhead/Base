#include "ManageUserWidget.h"
#include "ErrorMessage.h"

#include <QPainter>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QClipboard>
#include <QMenu>

#include <boost/chrono.hpp>
#include <boost/random.hpp>

#define ITEMS_PER_REQUEST 100

ManageUserWidget::ManageUserWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.userList->header()->setSortIndicator(0, Qt::AscendingOrder);

	QMenu* menu = new QMenu;
	ui_.setGroupButton->setMenu(menu);

	QAction* setUserGroupAction = menu->addAction("User Group");
	QAction* setAdminGroupAction = menu->addAction("Admin Group");

	QObject::connect(setUserGroupAction, &QAction::triggered, [this](){
		QList<QTreeWidgetItem*> items = ui_.userList->selectedItems();
		Rpc::ErrorCode ec;
		for (int i = 0; i < items.count(); ++i) {
			ec = context_->session->setUserGroup(items[i]->text(0).toStdString(), "User");
			if (ec == Rpc::ec_success) {
				items[i]->setText(1, "User");
			}
			else {
				context_->promptRpcError(ec);
				return;
			}
		}
		context_->promptRpcError(ec);
	});

	QObject::connect(setAdminGroupAction, &QAction::triggered, [this](){
		QList<QTreeWidgetItem*> items = ui_.userList->selectedItems();
		Rpc::ErrorCode ec;
		for (int i = 0; i < items.count(); ++i) {
			ec = context_->session->setUserGroup(items[i]->text(0).toStdString(), "Admin");
			if (ec == Rpc::ec_success) {
				items[i]->setText(1, "Admin");
			}
			else {
				context_->promptRpcError(ec);
				return;
			}
		}
		context_->promptRpcError(ec);
	});

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageUserWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageUserWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageUserWidget::onRefresh);
	//QObject::connect(ui_.removeButton, &QPushButton::clicked, this, &ManageUserWidget::onRemove);

	firstShow_ = true;
}

ManageUserWidget::~ManageUserWidget()
{
}

void ManageUserWidget::showEvent(QShowEvent* e)
{
	if (firstShow_) {
		onRefresh();
		firstShow_ = false;
	}
}

void ManageUserWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ManageUserWidget::onShowMore()
{
	if (!browser_) {
		return;
	}

	showMore(ITEMS_PER_REQUEST);
}

void ManageUserWidget::onShowAll()
{
	if (!browser_) {
		return;
	}

	showMore(std::numeric_limits<int>::max());
}

void ManageUserWidget::onRefresh()
{
	int count = 0;
	context_->session->onlineUserCount(count);
	ui_.infoLabel->setText(QString("%1 online").arg(count));

	ui_.userList->clear();
	context_->session->browseUsers(browser_);
	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}
}

void ManageUserWidget::showMore(int count)
{
	Rpc::ErrorCode ec;

	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

		Rpc::UserSeq users;
		browser_->next(n, users);

		QList<QTreeWidgetItem*> items;

		for (int i = 0; i < users.size(); ++i)
		{
			QStringList list;

			list << users[i].username.c_str();
			list << users[i].group.c_str();

			boost::replace_all(users[i].info, "\n", "\r");
			list << users[i].info.c_str();

			bool online;
			
			if ((ec = context_->session->isUserOnline(users[i].username, online)) != Rpc::ec_success) {
				context_->promptRpcError(ec);
				return;
			}

			list << (online ? "*" : "");

			QTreeWidgetItem* item = new QTreeWidgetItem(list);
			items.append(item);
		}

		ui_.userList->addTopLevelItems(items);

		count -= n;

		if (users.size() < n) {
			browser_ = 0;
			break;
		}
	}
}

