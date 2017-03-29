#include "CommentWidget.h"
#include "ui_CommentItemWidget.h"

#include <QResizeEvent>

#define ITEMS_PER_REQUEST 20

CommentWidget::CommentWidget(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	timer_ = new QTimer(this);

	QObject::connect(timer_, &QTimer::timeout, this, &CommentWidget::onTimeout);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &CommentWidget::onSubmit);

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
		Rpc::CommentSeq items;

		browser_->next(ITEMS_PER_REQUEST, items);

		for (const Rpc::Comment& item : items)
		{
			QWidget* w = new QWidget;

			Ui::CommentItemWidget ui;
			ui.setupUi(w);

			ui.userLabel->setText(item.user.c_str());
			ui.timeLabel->setText(item.time.c_str());
			ui.commentLabel->setText(item.comment.c_str());

			ui_.verticalLayout->addWidget(w);
		}

		if (items.size() < ITEMS_PER_REQUEST)
		{
			browser_ = 0;
			timer_->stop();
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

void CommentWidget::refresh()
{
	for (;;)
	{
		QLayoutItem* li = ui_.verticalLayout->takeAt(0);

		if (!li) {
			break;
		}

		li->widget()->deleteLater();

		delete li;
	}

	if (context_)
	{
		context_->session->browseComment(targetId_.toStdString(), "", browser_);

		if (browser_)
		{
			timer_->start();
		}
	}
}

