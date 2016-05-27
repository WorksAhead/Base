#include "ManageEngine.h"
#include "SubmitEngineDialog.h"
#include "ErrorMessage.h"

#include <Crc.h>

#include <QPainter>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <numeric>

#define ITEMS_PER_REQUEST 100

#include "ASyncTask.h"
#include <boost/thread.hpp>
#include <thread>
#include <memory>

class FooTask : public ASyncTask {
public:
	FooTask()
	{
		state_ = ASyncTask::state_idle;
		progress_ = 0;
		info_ = "FooTask";
		cancel_ = false;
	}

	~FooTask()
	{
		if (t_->joinable()) {
			t_->join();
		}
	}

	virtual void start()
	{
		t_.reset(new std::thread(std::bind(&FooTask::foo, this)));
	}

	virtual void cancel()
	{
		boost::unique_lock<boost::mutex> lock(sync_);
		if (cancel_ || state_ != ASyncTask::state_running) {
			return;
		}
		cancel_ = true;
		lock.unlock();
		t_->join();
	}

	virtual int state()
	{
		boost::mutex::scoped_lock lock(sync_);
		return state_;
	}

	virtual int progress()
	{
		boost::mutex::scoped_lock lock(sync_);
		return progress_;
	}

	virtual std::string information()
	{
		boost::mutex::scoped_lock lock(sync_);
		return info_;
	}

private:
	void foo()
	{
		sync_.lock();
		state_ = ASyncTask::state_running;
		sync_.unlock();

		for (int i = 0; i < 100; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			boost::mutex::scoped_lock lock(sync_);
			if (cancel_) {
				state_ = ASyncTask::state_cancelled;
				return;
			}
			++progress_;
		}

		sync_.lock();
		state_ = ASyncTask::state_finished;
		sync_.unlock();
	}

private:
	int state_;
	int progress_;
	std::string info_;
	bool cancel_;
	boost::mutex sync_;
	std::shared_ptr<std::thread> t_;
};

ManageEngine::ManageEngine(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	ui_.setupUi(this);
	ui_.endTimeEdit->setDateTime(QDateTime::currentDateTime());

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
		context_->session->browseEngines(browser_);
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
	context_->session->browseEngines(browser_);
	showMore(ITEMS_PER_REQUEST);
}

void ManageEngine::onRemove()
{
	const int rc = QMessageBox::question(
		0, "Base",
		"Are you sure you want to remove these versions ?\nWarning: This operation cannot be undone.",
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (rc != QMessageBox::Yes) {
		return;
	}

	QList<QTreeWidgetItem*> items = ui_.engineList->selectedItems();
	for (int i = 0; i < items.count(); ++i) {
		context_->session->removeEngine(items[i]->text(0).toStdString(), items[i]->text(1).toStdString());
	}
}

void ManageEngine::showSubmitDialog()
{
	context_->addTask(new FooTask);

	/*SubmitEngineDialog d;

	if (d.exec() == 1)
	{
		Rpc::EngineUploaderPrx engineUploader;
		Rpc::ErrorCode ec = context_->session->uploadEngine(d.engine().toStdString(), d.version().toStdString(), d.info().toStdString(), engineUploader);
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
	}*/
}

void ManageEngine::showMore(int count)
{
	while (count > 0)
	{
		const int n = std::min(count, ITEMS_PER_REQUEST);

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

