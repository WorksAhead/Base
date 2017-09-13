#ifndef CONTENTDOWNLOADDIALOG_HEADER_
#define CONTENTDOWNLOADDIALOG_HEADER_

#include "ui_ContentDownloadDialog.h"

class ContentDownloadDialog : public QDialog {
private:
	Q_OBJECT

public:
	ContentDownloadDialog(QWidget* parent = 0);
	~ContentDownloadDialog();

	void setEngineVersionAboutToBeDownloaded(const QString& name, const QString& version);
	bool isInstallEngineChecked();

private Q_SLOTS:
	void onDownload();

private:
	Ui::ContentDownloadDialog ui_;
};

#endif // CONTENTDOWNLOADDIALOG_HEADER_

