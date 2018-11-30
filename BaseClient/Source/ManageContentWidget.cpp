#include "ManageContentWidget.h"
#include "ErrorMessage.h"
#include "SubmitContentDialog.h"

#include <QPainter>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <algorithm>

#define ITEMS_PER_REQUEST 100

ManageContentWidget::ManageContentWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.contentList->header()->setSortIndicator(12, Qt::DescendingOrder);

	QObject::connect(ui_.showMoreButton, &QPushButton::clicked, this, &ManageContentWidget::onShowMore);
	QObject::connect(ui_.showAllButton, &QPushButton::clicked, this, &ManageContentWidget::onShowAll);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &ManageContentWidget::onRefresh);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &ManageContentWidget::onSubmit);
	QObject::connect(ui_.editButton, &QPushButton::clicked, this, &ManageContentWidget::onEdit);
	QObject::connect(ui_.changeDisplayPriorityButton, &QPushButton::clicked, this, &ManageContentWidget::onChangeDisplayPriority);
	QObject::connect(ui_.changeStateButton, &QToolButton::clicked, ui_.changeStateButton, &QToolButton::showMenu);

	if (context_->currentUserGroup == "Admin") {
		ui_.changeDisplayPriorityButton->setEnabled(true);
	}

	ui_.changeStateButton->setMenu(new QMenu);
	{
		QMenu* menu = ui_.changeStateButton->menu();

		QAction* a1 = menu->addAction("Normal");
		QAction* a2 = menu->addAction("Removed");
		QAction* a3 = menu->addAction("Hidden");

		QObject::connect(a1, &QAction::triggered, [this]()
		{
			QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();

			for (int i = 0; i < items.count(); ++i)
			{
				context_->session->changeContentState(items[i]->text(0).toStdString(), "Normal");
			}
		});

		QObject::connect(a2, &QAction::triggered, [this]()
		{
			QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();

			for (int i = 0; i < items.count(); ++i)
			{
				context_->session->changeContentState(items[i]->text(0).toStdString(), "Removed");
			}
		});

		QObject::connect(a3, &QAction::triggered, [this]()
		{
			QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();

			for (int i = 0; i < items.count(); ++i)
			{
				context_->session->changeContentState(items[i]->text(0).toStdString(), "Hidden");
			}
		});
	}

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

	context_->session->browseContent("", "", "", browser_);

	if (browser_) {
		showMore(ITEMS_PER_REQUEST);
	}

	queryEngineVersions(true);
}

void ManageContentWidget::onSubmit()
{
	bool copyForm = false;
	bool copyContent = false;
	bool newVersion = false;

	QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();

	if (items.count() == 1)
	{
		QMessageBox msgBox(this);

		msgBox.setWindowTitle("Base");

		QPushButton* b0 = msgBox.addButton("New Content", QMessageBox::NoRole);
		QPushButton* b1 = msgBox.addButton("New Version of selected Content", QMessageBox::NoRole);
		QPushButton* b2 = msgBox.addButton("Copy selected Content", QMessageBox::NoRole);
		QPushButton* b3 = msgBox.addButton("Cancel", QMessageBox::NoRole);

		msgBox.exec();

		if (msgBox.clickedButton() == b3) {
			return;
		}
		else if (msgBox.clickedButton() == b2) {
			copyForm = true;
			copyContent = true;
		}
		else if (msgBox.clickedButton() == b1) {
			copyForm = true;
			newVersion = true;
		}
	}

	SubmitContentDialog d(context_, this);

	d.setEngineVersions(queryEngineVersions());

	if (copyForm)
	{
		Rpc::ErrorCode ec;

		Rpc::ContentInfo ci;

		if ((ec = context_->session->getContentInfo(items[0]->text(0).toStdString(), ci)) != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		if (copyContent) {
			d.switchToCopyMode(ci.id.c_str());
		}

		d.loadImagesFrom(ci.id.c_str(), ci.imageCount);

		std::string command;
		std::string workDir;
		std::istringstream stream(ci.startup);
		getline(stream, command);
		getline(stream, workDir);

		if (newVersion) {
			d.setParentId(ci.parentId.empty() ? ci.id.c_str() : ci.parentId.c_str());
		}
		else {
			d.setParentId(ci.parentId.c_str());
		}

		d.setTitle(ci.title.c_str());
		d.setPage(ci.page.c_str());
		d.setCategory(ci.category.c_str());
		d.setEngineNameAndVersion(ci.engineName.c_str(), ci.engineVersion.c_str());
		d.setCommand(command.c_str());
		d.setWorkingDir(workDir.c_str());
		d.setVideo(ci.video.c_str());
		d.setDesc(ci.desc.c_str());
	}

	d.exec();
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

	d.setEngineVersions(queryEngineVersions());

	d.switchToEditMode(ci.id.c_str());

	d.setParentId(ci.parentId.c_str());
	d.setTitle(ci.title.c_str());
	d.setPage(ci.page.c_str());
	d.setCategory(ci.category.c_str());
	d.setEngineNameAndVersion(ci.engineName.c_str(), ci.engineVersion.c_str());
	d.setCommand(command.c_str());
	d.setWorkingDir(workDir.c_str());
	d.setVideo(ci.video.c_str());
	d.setDesc(ci.desc.c_str());

	d.exec();
}

void ManageContentWidget::onChangeDisplayPriority()
{
	QList<QTreeWidgetItem*> items = ui_.contentList->selectedItems();

	if (items.count() == 0) {
		return;
	}

	int oldValue = 0;

	if (items.count() == 1)
	{
		Rpc::ContentInfo ci;

		Rpc::ErrorCode ec;

		if ((ec = context_->session->getContentInfo(items[0]->text(0).toStdString(), ci)) != Rpc::ec_success)
		{
			context_->promptRpcError(ec);
			return;
		}

		oldValue = ci.displayPriority;
	}

	bool ok;

	int displayPriority = QInputDialog::getInt(this, "Base", "DisplayPriority", oldValue, -2147483647, 2147483647, 1, &ok, Qt::WindowTitleHint);

	if (!ok) {
		return;
	}

	for (int i = 0; i < items.count(); ++i)
	{
		context_->session->changeContentDisplayPriority(items[i]->text(0).toStdString(), displayPriority);
	}
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

			if (ci.state == "Deleted") {
				continue;
			}
			else if (ci.state == "Normal" && !ui_.showNormalCheckBox->isChecked()) {
				continue;
			}
			else if (ci.state == "Hidden" && !ui_.showHiddenCheckBox->isChecked()) {
				continue;
			}
			else if (ci.state == "Removed" && !ui_.showRemovedCheckBox->isChecked()) {
				continue;
			}

			std::string::size_type atPos = ci.user.find('@');
			if (atPos != std::string::npos) {
				ci.user.erase(atPos);
			}

			if (!boost::iequals(context_->currentUser, ci.user) && context_->currentUserGroup != "Admin") {
				continue;
			}

			std::string command;
			std::string workDir;
			std::istringstream stream(ci.startup);
			getline(stream, command);
			getline(stream, workDir);

			boost::replace_all(ci.title, "\r", " ");
			boost::replace_all(ci.video, "\n", "\r");
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
			list << ci.video.c_str();
			list << ci.desc.c_str();
			list << ci.user.c_str();
			list << ci.upTime.c_str();
			list << QString("%1").arg(ci.displayPriority);
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

QStringList ManageContentWidget::queryEngineVersions(bool refresh)
{
	if (refresh)
	{
		cachedEngineVersions_.clear();

		Rpc::EngineVersionBrowserPrx browser;

		context_->session->browseEngineVersions(false, browser);

		if (browser)
		{
			std::vector<std::string> v;

			for (;;)
			{
				Rpc::EngineVersionSeq items;

				browser->next(ITEMS_PER_REQUEST, items);

				for (size_t i = 0; i < items.size(); ++i)
				{
					v.push_back(items[i].name + "\r" + items[i].version);
				}

				if (items.size() < ITEMS_PER_REQUEST) {
					break;
				}
			}

			std::stable_sort(v.begin(), v.end());

			for (size_t i = 0; i < v.size(); ++i)
			{
				cachedEngineVersions_.append(v[i].c_str());
			}
		}
	}

	return cachedEngineVersions_;
}

