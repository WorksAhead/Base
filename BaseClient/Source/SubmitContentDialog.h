#ifndef SUBMITCONTENTDIALOG_HEADER_
#define SUBMITCONTENTDIALOG_HEADER_

#include "Context.h"
#include "Script.h"

#include "ui_SubmitContentDialog.h"

#include <QStringList>

class SubmitContentDialog : public QDialog {
private:
	Q_OBJECT

public:
	SubmitContentDialog(ContextPtr context, QWidget* parent = 0);
	~SubmitContentDialog();

	void setEngineVersions(const QStringList& engineVersions);

	void switchToEditMode(const QString& contentId);
	void switchToCopyMode(const QString& contentId);

	void loadImagesFrom(const QString& contentId, int count);

	void setParentId(const QString& parentId);
	void setTitle(const QString& title);
	void setPage(const QString& name);
	void setCategory(const QString& category);
	void setEngineNameAndVersion(const QString& name, const QString& version);
	void setCommand(const QString& command);
	void setWorkingDir(const QString& workDir);
	void setVideo(const QString& video);
	void setDesc(const QString& desc);

private Q_SLOTS:
	void onShowAll();
	void onEditPage();
	void onEditCategory();
	void onAddEngineVersion();
	void onRemoveEngineVersion();
	void onBrowseLocation();
	void onBrowseProjectLocation();
	void onSetCover();
	void onPrevScreenshot();
	void onNextScreenshot();
	void onAddScreenshot();
	void onRemoveScreenshot();
	void onSubmit();

	void onImageLoaded(const QString& id, int index, const QPixmap&);

protected:
	virtual void keyPressEvent(QKeyEvent*);

private:
	void generateCommandAndWorkDir();
	bool checkLocation();
	QPixmap getImage(const QSize& ratio);
	QString getOpenImageFileName();

private:
	ContextPtr context_;
	QStringList engineVersions_;
	bool editMode_;
	bool copyMode_;
	QString contentId_;
	Ui::SubmitContentDialog ui_;
	Script script_;
};

#endif // SUBMITCONTENTDIALOG_HEADER_

