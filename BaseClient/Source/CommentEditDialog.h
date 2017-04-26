#ifndef COMMENTEDITDIALOG_HEADER_
#define COMMENTEDITDIALOG_HEADER_

#include "ui_CommentEditDialog.h"

class CommentEditDialog : public QDialog {
private:
	Q_OBJECT

public:
	CommentEditDialog(QWidget* parent = 0);
	~CommentEditDialog();

	void setHtml(const QString&);
	QString toHtml();

private Q_SLOTS:
	void onAccept();
	void onReject();

private:
	Ui::CommentEditDialog ui_;

	QString html_;
};

#endif // COMMENTEDITDIALOG_HEADER_

