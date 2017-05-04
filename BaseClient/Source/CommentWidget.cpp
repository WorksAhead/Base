#include "CommentWidget.h"
#include "Emoji.h"
#include "CommentEditDialog.h"

#include <QResizeEvent>
#include <QTextDocument>
#include <QTextBlock>
#include <QMessageBox>

#include <gumbo.h>

#define ITEMS_PER_REQUEST 20

CommentWidget::CommentWidget(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	timer_ = new QTimer(this);

	QObject::connect(timer_, &QTimer::timeout, this, &CommentWidget::onTimeout);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &CommentWidget::onSubmit);
	QObject::connect(ui_.commentBrowser, &QTextBrowser::anchorClicked, this, &CommentWidget::onAnchorClicked);

	browser_ = 0;
}

CommentWidget::~CommentWidget()
{

}

void CommentWidget::setContext(ContextPtr context)
{
	context_ = context;
}

void CommentWidget::setTargetId(const QString& targetId)
{
	if (targetId != targetId_)
	{
		targetId_ = targetId;
		refresh();
	}
}

void CommentWidget::resizeEvent(QResizeEvent* e)
{
}

void CommentWidget::onTimeout()
{
	if (browser_)
	{
		QTextBlockFormat format;

		format.setLeftMargin(12);
		format.setRightMargin(12);
		format.setTopMargin(12);
		format.setBottomMargin(12);

		Rpc::CommentSeq items;

		browser_->next(ITEMS_PER_REQUEST, items);

		if (!items.empty())
		{
			ui_.commentBrowser->setVisible(true);
		}

		for (Rpc::Comment& item : items)
		{
			std::string::size_type atPos = item.user.find('@');

			if (atPos != std::string::npos) {
				item.user.erase(atPos);
			}

			QTextCursor cursor = ui_.commentBrowser->textCursor();

			if (ui_.commentBrowser->document()->blockCount() > 1) {
				cursor.insertBlock();
			}

			cursor.insertHtml(QString::fromStdString("<p>" + item.user + " " + item.time + "</p>"));
			format.setLeftMargin(12);
			cursor.setBlockFormat(format);

			GumboOutput* output = gumbo_parse(item.comment.c_str());

			QStringList paragraphs;

			getParagraphs(output->root, paragraphs);

			gumbo_destroy_output(&kGumboDefaultOptions, output);

			for (QString& p : paragraphs)
			{
				cursor.insertBlock();
				cursor.insertHtml(p);
				format.setLeftMargin(12*6);
				cursor.setBlockFormat(format);
			}

			if (item.user == context_->currentUser)
			{
				cursor.insertBlock();
				cursor.insertHtml(QString::fromStdString("<p><a href=\"Edit#" + item.id + "\">Edit</a> <a href=\"Delete#" + item.id + "\">Delete</a></p>"));
				format.setLeftMargin(12*6);
				cursor.setBlockFormat(format);
			}
			else if (context_->currentUserGroup == "Admin")
			{
				cursor.insertBlock();
				cursor.insertHtml(QString::fromStdString("<p><a href=\"Delete#" + item.id + "\">Delete</a></p>"));
				format.setLeftMargin(12*6);
				cursor.setBlockFormat(format);
			}

			cursor.insertBlock();
		}

		QSize size = ui_.commentBrowser->document()->size().toSize();
		ui_.commentBrowser->setFixedHeight(size.height() + 12);

		if (items.size() < ITEMS_PER_REQUEST)
		{
			browser_ = 0;
			timer_->stop();

			QString ss = ui_.commentBrowser->toHtml();
			ss.toStdString();
		}
	}
}

void CommentWidget::onSubmit()
{
	if (context_)
	{
		if (!ui_.textEdit->toHtml().isEmpty())
		{
			Rpc::ErrorCode ec = context_->session->addComment(targetId_.toStdString(), ui_.textEdit->toHtml().toStdString());

			context_->promptRpcError(ec);

			if (ec == Rpc::ec_success)
			{
				ui_.textEdit->clear();
				refresh();
			}
		}
	}
}

void CommentWidget::onAnchorClicked(const QUrl& url)
{
	QString path = url.path();
	QString fragment = url.fragment();

	if (path == "Delete")
	{
		int rc = QMessageBox::question(this, "Base",
			"Are you sure you want to delete this Comment ?\nWarning: This operation cannot be undone.",
			QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

		if (rc == QMessageBox::Yes)
		{
			Rpc::ErrorCode ec = context_->session->removeComment(fragment.toStdString());
			context_->promptRpcError(ec);
			refresh();
		}
	}
	else if (path == "Edit")
	{
		std::string comment;

		Rpc::ErrorCode ec = context_->session->getComment(fragment.toStdString(), comment);

		if (ec != Rpc::ec_success) {
			context_->promptRpcError(ec);
			return;
		}

		CommentEditDialog d(this);

		d.setHtml(comment.c_str());

		if (d.exec() == 1)
		{
			ec = context_->session->editComment(fragment.toStdString(), d.toHtml().toStdString());

			context_->promptRpcError(ec);

			if (ec == Rpc::ec_success) {
				refresh();
			}
		}
	}
	else
	{
		context_->openUrl(url.toString().toStdString());
	}
}

void CommentWidget::refresh()
{
	ui_.commentBrowser->clear();

	ui_.commentBrowser->setVisible(false);

	addEmojiResourcesToDocument(ui_.commentBrowser->document());

	if (context_)
	{
		context_->session->browseComment(targetId_.toStdString(), "", browser_);

		if (browser_)
		{
			timer_->start();
		}
	}
}

void CommentWidget::getParagraphs(void* p, QStringList& paragraphs)
{
	GumboNode* node = (GumboNode*)p;

	if (node->type == GUMBO_NODE_ELEMENT)
	{
		if (node->v.element.tag == GUMBO_TAG_P)
		{
			GumboStringPiece beginTag = node->v.element.original_tag;
			GumboStringPiece endTag = node->v.element.original_end_tag;

			const char* begin = beginTag.data + beginTag.length;
			const char* end = endTag.data;

			QString s = QString::fromUtf8(begin, end - begin);

			if (s != "<br />")
			{
				paragraphs.append("<p>" + s + "</p>");
			}
		}
		else if (node->v.element.tag != GUMBO_TAG_SCRIPT && node->v.element.tag != GUMBO_TAG_STYLE)
		{
			GumboVector* children = &node->v.element.children;

			for (unsigned int i = 0; i < children->length; ++i)
			{
				getParagraphs((GumboNode*)children->data[i], paragraphs);
			}
		}
	}
}

