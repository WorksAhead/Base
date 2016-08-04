#ifndef SUBMITCONTENTDIALOG_HEADER_
#define SUBMITCONTENTDIALOG_HEADER_

#include "Context.h"

#include "ui_SubmitContentDialog.h"

class SubmitContentDialog : public QDialog {
private:
	Q_OBJECT

public:
	SubmitContentDialog(ContextPtr context, QWidget* parent = 0);
	~SubmitContentDialog();

	void setPage(const QString& name);

private Q_SLOTS:
	void onSelectLocation();
	void onSetCover();
	void onPrevScreenshot();
	void onNextScreenshot();
	void onAddScreenshot();
	void onRemoveScreenshot();
	void onSubmit();

protected:
	virtual void keyPressEvent(QKeyEvent*);

private:
	QPixmap getImage(const QSize& ratio);
	QString getOpenImageFileName();

private:
	ContextPtr context_;

	Ui::SubmitContentDialog ui_;
};

#endif // SUBMITCONTENTDIALOG_HEADER_

