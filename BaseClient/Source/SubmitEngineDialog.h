#ifndef SUBMITENGINEDIALOG_HEADER_
#define SUBMITENGINEDIALOG_HEADER_

#include "ui_SubmitEngineDialog.h"

#include <QString>

class SubmitEngineDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit SubmitEngineDialog(QWidget* parent = 0);
	~SubmitEngineDialog();

	QString engine() const { return engine_; }
	QString version() const { return version_; }
	QString path() const { return path_; }
	QString info() const { return info_; }

private Q_SLOTS:
	void selectPath();
	void submit();

private:
	Ui::SubmitEngineDialog ui_;
	QString engine_;
	QString version_;
	QString path_;
	QString info_;
};

#endif // SUBMITENGINEDIALOG_HEADER_

