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
	void onAnchorClicked(const QUrl&);
	void onTimeout();

protected:
	void showEvent(QShowEvent*);

private:
	ContextPtr context_;

	QTimer* timer_;

	Rpc::ContentBrowserPrx browser_;

	Ui::HistoryDialog ui_;

	std::string lastViewTime_;
};

#endif // HISTORYDIALOG_HEADER

