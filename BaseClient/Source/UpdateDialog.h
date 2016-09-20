#ifndef UPDATEDIALOG_HEADER_
#define UPDATEDIALOG_HEADER_

#include <ui_UpdateDialog.h>

#include <QTimer>
#include <QString>

// forward declaration
class ASyncTaskWidget;

class UpdateDialog : public QDialog {
private:
	Q_OBJECT

public:
	UpdateDialog(const QString& sourceDir, const QString& targetDir, QWidget* parent = 0);
	~UpdateDialog();

private Q_SLOTS:
	void onBegin();
	void onRefresh();

private:
	void beginUpdate();

private:
	QString sourceDir_;
	QString targetDir_;
	Ui::UpdateDialog ui_;
	ASyncTaskWidget* taskWidget_;
	QTimer* beginTimer_;
	QTimer* refreshTimer_;
};

#endif // UPDATEDIALOG_HEADER_

