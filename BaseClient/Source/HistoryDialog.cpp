#include "HistoryDialog.h"
#include "Datetime.h"
#include "URLUtils.h"

#include <QTextDocument>
#include <QTextBlock>

#include <boost/algorithm/string.hpp>

#define ITEMS_PER_REQUEST 20

HistoryDialog::HistoryDialog(ContextPtr context, QWidget* parent) : QDialog(parent), context_(context)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	timer_ = new QTimer(this);
	timer_->setInterval(100);

	QObject::connect(ui_.content, &QTextBrowser::anchorClicked, this, &HistoryDialog::onAnchorClicked);
	QObject::connect(timer_, &QTimer::timeout, this, &HistoryDialog::onTimeout);
}

HistoryDialog::~HistoryDialog()
{
}

void HistoryDialog::onAnchorClicked(const QUrl& url)
{
	context_->openUrl(url.toString().toStdString());
}

void HistoryDialog::onTimeout()
{
	Rpc::ContentItemSeq items;

	if (browser_->next(ITEMS_PER_REQUEST, items) != Rpc::ec_success) {
		return;
	}

	if (items.empty())
	{
		timer_->stop();
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

			QTextCursor cursor = ui_.content->textCursor();

			QString html;

			if (!lastViewTime_.empty() && ci.upTime <= lastViewTime_)
			{
				lastViewTime_.clear();

				html += "<hr />";
			}

			boost::replace_all(ci.title, "\r", " ");

			std::vector<std::string> pages;
			boost::split(pages, ci.page, boost::is_any_of(","));

			if (pages.empty()) {
				continue;
			}

			percentEncode(pages[0]);

			html += QString::fromStdString("<p><a href=\"base://content/?id=" + ci.id +
				"&page=" + pages[0] + "\">" +
				ci.title + "</a>" + "<br />" + ci.upTime + "</p>");

			cursor.insertHtml(html);
			cursor.insertBlock();
		}
	}
}

void HistoryDialog::showEvent(QShowEvent* e)
{
	QDialog::showEvent(e);

	lastViewTime_ = context_->getLastViewTime();

	//context_->setLastViewTime(getCurrentTimeString());

	if (context_->session->browseContent("", "", "", browser_) != Rpc::ec_success) {
		return;
	}

	ui_.content->clear();

	timer_->start();
}

