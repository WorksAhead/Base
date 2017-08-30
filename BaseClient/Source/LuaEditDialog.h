#ifndef LUAEDITDIALOG_HEADER_
#define LUAEDITDIALOG_HEADER_

#include "ui_LuaEditDialog.h"

class LuaEditDialog : public QDialog {
private:
	Q_OBJECT

public:
	LuaEditDialog(const QString& text, QWidget* parent = 0);
	~LuaEditDialog();

	QString text();

private Q_SLOTS:
	void onSubmit();

private:
	Ui::LuaEditDialog ui_;
};

#endif // LUAEDITDIALOG_HEADER_

