#include "ManageEngine.h"
#include "SubmitEngineDialog.h"
#include "ErrorMessage.h"

#include <Crc.h>

#include <QPainter>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <numeric>

ManageEngine::ManageEngine(Rpc::SessionPrx session, QWidget* parent) : QWidget(parent), session_(session)
{
	ui_.setupUi(this);
	ui_.endTimeEdit->setDateTime(QDateTime::currentDateTime());

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
	if (!browser_) {
		session_->browseEngines(browser_);
		showMore(1);
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

	showMore(1);
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
	session_->browseEngines(browser_);
	showMore(1);
}

void ManageEngine::onRemove()
{
	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		session_->removeEngine(items[i]->text(0).toStdString(), items[i]->text(1).toStdString());
	}
}

void ManageEngine::showSubmitDialog()
{
	SubmitEngineDialog d;

	if (d.exec() == 1)
	{
		Rpc::EngineUploaderPrx engineUploader;
		Rpc::ErrorCode ec = session_->uploadEngine(d.engine().toStdString(), d.version().toStdString(), d.info().toStdString(), engineUploader);
		if (ec != Rpc::ec_success) {
			QMessageBox msg;
			msg.setWindowTitle("Base");
			msg.setText(errorMessage(ec));
			msg.exec();
			return;
		}

		Crc32 crc;

		char buf[1024];

		for (int i = 0; i < 100; ++i) {
			crc.update(buf, sizeof(buf));
			ec = engineUploader->write(i * sizeof(buf), std::make_pair((const Ice::Byte*)buf, (const Ice::Byte*)buf + sizeof(buf)));
			if (ec != Rpc::ec_success) {
				QMessageBox msg;
				msg.setWindowTitle("Base");
				msg.setText(errorMessage(ec));
				msg.exec();
				return;
			}
		}

		ec = engineUploader->finish(crc.value());
		if (ec != Rpc::ec_success) {
			QMessageBox msg;
			msg.setWindowTitle("Base");
			msg.setText(errorMessage(ec));
			msg.exec();
			return;
		}

		engineUploader_ = engineUploader;
	}
}

void ManageEngine::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, 100);

		Rpc::EngineItemSeq engineItems;
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

