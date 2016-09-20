#ifndef DOWNLOADCLIENTDIALOG_HEADER_
#define DOWNLOADCLIENTDIALOG_HEADER_

#include <ui_DownloadClientDialog.h>

#include <QTimer>

#include <RpcStart.h>
#include <RpcSession.h>

// forward declaration
class ASyncTaskWidget;

class DownloadClientDialog : public QDialog {
private:
	Q_OBJECT

public:
	DownloadClientDialog(Rpc::StartPrx, QWidget* parent = 0);
	~DownloadClientDialog();

private Q_SLOTS:
	void onRefresh();

private:
	bool beginDownload();

private:
	Ui::DownloadClientDialog ui_;
	Rpc::StartPrx startPrx_;
	Rpc::DownloaderPrx downloaderPrx_;
	ASyncTaskWidget* taskWidget_;
	QTimer* refreshTimer_;
};

#endif // DOWNLOADCLIENTDIALOG_HEADER_

