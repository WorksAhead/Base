#ifndef LABELSELECTORDIALOG_HEADER_
#define LABELSELECTORDIALOG_HEADER_

#include <ui_LabelSelectorDialog.h>

#include <QDialog>

class LabelSelectorDialog : public QDialog {
private:
	Q_OBJECT

public:
	LabelSelectorDialog(QWidget* parent = 0);
	~LabelSelectorDialog();

	LabelSelectorWidget* labelSelectorWidget();

private:
	Ui::LabelSelectorDialog ui_;
};

#endif // LABELSELECTORDIALOG_HEADER_
