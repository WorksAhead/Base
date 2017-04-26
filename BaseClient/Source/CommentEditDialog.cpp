#include "CommentEditDialog.h"

#include <QMessageBox>

CommentEditDialog::CommentEditDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.buttonBox, &QDialogButtonBox::accepted, this, &CommentEditDialog::onAccept);
	QObject::connect(ui_.buttonBox, &QDialogButtonBox::rejected, this, &CommentEditDialog::onReject);
}

CommentEditDialog::~CommentEditDialog()
{
}

void CommentEditDialog::setHtml(const QString& html)
{
	html_ = html;
	ui_.textEdit->setHtml(html);
}

QString CommentEditDialog::toHtml()
{
	return ui_.textEdit->toHtml();
}

void CommentEditDialog::onAccept()
{
	done(1);
}

void CommentEditDialog::onReject()
{
	if (toHtml() != html_)
	{
		int rc = QMessageBox::question(this, "Base",
			"Are you sure you want to discard changes ?",
			QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

		if (rc == QMessageBox::Yes)
		{
			done(0);
		}
	}
	else
	{
		done(0);
	}
}

