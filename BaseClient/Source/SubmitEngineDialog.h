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

	QString engineName() const { return engineName_; }
	QString engineVersion() const { return engineVersion_; }
	QString location() const { return location_; }
	QString info() const { return info_; }

private Q_SLOTS:
	void selectLocation();
	void submit();

private:
	Ui::SubmitEngineDialog ui_;
	QString engineName_;
	QString engineVersion_;
	QString location_;
	QString info_;
};

#endif // SUBMITENGINEDIALOG_HEADER_

