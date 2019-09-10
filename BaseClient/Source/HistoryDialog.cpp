#include "HistoryDialog.h"
#include "Datetime.h"
#include "Base64Url.h"
#include "URLUtils.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QScrollBar>

#include <boost/algorithm/string.hpp>

#define ITEMS_FIRST_REQUEST 60
#define ITEMS_PER_REQUEST 20

HistoryDialog::HistoryDialog(ContextPtr context, QWidget* parent) : QDialog(parent), context_(context)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	timer_ = new QTimer(this);
	timer_->setInterval(100);

	QObject::connect(ui_.markButton, &QPushButton::clicked, this, &HistoryDialog::onMark);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &HistoryDialog::onRefresh);
	QObject::connect(ui_.content->verticalScrollBar(), &QScrollBar::valueChanged, this, &HistoryDialog::onScroll);
	QObject::connect(ui_.content, &QTextBrowser::anchorClicked, this, &HistoryDialog::onAnchorClicked);
	QObject::connect(timer_, &QTimer::timeout, this, &HistoryDialog::onTimeout);

	lastViewStamp_ = 0;
	latestTime_ = 0;
}

HistoryDialog::~HistoryDialog()
{
}

void HistoryDialog::onMark()
{
	if (latestTime_)
	{
		context_->setLastViewStamp(latestTime_);
	}
}

void HistoryDialog::onRefresh()
{
	ui_.content->clear();

	latestTime_ = 0;

	lastViewStamp_ = context_->getLastViewStamp();

	if (context_->session->browseContent("", "", "", browser_) != Rpc::ec_success) {
		return;
	}

	count_ = 0;

	showMore(ITEMS_FIRST_REQUEST);
}

void HistoryDialog::onAnchorClicked(const QUrl& url)
{
	context_->openUrl(url.toString().toStdString());
}

void HistoryDialog::onScroll(int)
{
	QScrollBar* bar = ui_.content->verticalScrollBar();

	if (bar->value() >= bar->maximum() / 10 * 9)
	{
		if (browser_) {
			showMore(ITEMS_PER_REQUEST);
		}
	}
}

void HistoryDialog::onTimeout()
{
	if (count_ > 0 && browser_)
	{
		const int n = qMin(count_, 5);
		int m = 0;

		Rpc::ContentItemSeq items;

		if (browser_->next(n, items) != Rpc::ec_success) {
			return;
		}

		for (size_t i = 0; i < items.size(); ++i)
		{
			if (items[i].state == "Normal")
			{
				Rpc::ContentInfo ci;

				if (context_->session->getContentInfo(items[i].id, ci) != Rpc::ec_success) {
					continue;
				}

				if (!latestTime_) {
					latestTime_ = ci.rowid;
				}

				QString html;

				if (lastViewStamp_ && ci.rowid <= lastViewStamp_)
				{
					lastViewStamp_ = 0;

					html += "<hr />";
				}

				boost::replace_all(ci.title, "\r", " ");

				std::vector<std::string> pages;
				boost::split(pages, ci.page, boost::is_any_of(","));

				if (pages.empty()) {
					continue;
				}

				html += QString::fromStdString("<p><a href=\"base://content/?id=" + ci.id +
					"&page=" + base64url_encode(pages[0]) + "\">" +
					ci.title + "</a>" + "<br />" + ci.upTime + "</p>");

				QTextCursor cursor(ui_.content->document());

				cursor.movePosition(QTextCursor::End);
				cursor.insertHtml(html);
				cursor.insertBlock();

				++m;
			}
		}

		count_ -= m;

		if (items.size() < n)
		{
			browser_ = 0;
			count_ = 0;
			timer_->stop();
		}
	}
}

void HistoryDialog::showEvent(QShowEvent* e)
{
	QDialog::showEvent(e);

	onRefresh();
}

void HistoryDialog::closeEvent(QCloseEvent*)
{
	browser_ = 0;
	timer_->stop();
}

void HistoryDialog::showMore(int n)
{
	count_ += n;
	timer_->start();
}

