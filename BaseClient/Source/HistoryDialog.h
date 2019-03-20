#ifndef HISTORYDIALOG_HEADER
#define HISTORYDIALOG_HEADER

#include "Context.h"

#include "ui_HistoryDialog.h"

#include <QTimer>

#include <string>

class HistoryDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit HistoryDialog(ContextPtr context, QWidget* parent = 0);
	~HistoryDialog();

private Q_SLOTS:
	void onMark();
	void onRefresh();
	void onAnchorClicked(const QUrl&);
	void onScroll(int);
	void onTimeout();

protected:
	void showEvent(QShowEvent*);
	void closeEvent(QCloseEvent*);

private:
	void showMore(int);

private:
	ContextPtr context_;

	QTimer* timer_;

	int count_;

	Rpc::ContentBrowserPrx browser_;

	Ui::HistoryDialog ui_;

	int64_t lastViewStamp_;
	int64_t latestTime_;
};

#endif // HISTORYDIALOG_HEADER

