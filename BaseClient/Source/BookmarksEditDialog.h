#ifndef BOOKMARKSEDITDIALOG_HEADER_
#define BOOKMARKSEDITDIALOG_HEADER_

#include "ui_BookmarksEditDialog.h"

class BookmarksEditDialog : public QDialog {
private:
	Q_OBJECT

public:
	BookmarksEditDialog(const QString& text, QWidget* parent = 0);
	~BookmarksEditDialog();

	QString text();

	private Q_SLOTS:
	void onSubmit();

private:
	Ui::BookmarksEditDialog ui_;
};

#endif // BOOKMARKSEDITDIALOG_HEADER_

