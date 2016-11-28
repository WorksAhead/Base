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

	void switchToEditMode(const QString& contentId);

	void setParentId(const QString& parentId);
	void setTitle(const QString& title);
	void setPage(const QString& name);
	void setCategory(const QString& category);
	void setEngineName(const QString& name);
	void setEngineVersion(const QString& version);
	void setCommand(const QString& command);
	void setWorkingDir(const QString& workDir);
	void setDesc(const QString& desc);

private Q_SLOTS:
	void onEditPage();
	void onEditCategory();
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
	bool editMode_;
	QString contentId_;
	Ui::SubmitContentDialog ui_;
};

#endif // SUBMITCONTENTDIALOG_HEADER_

