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

	void switchToEditMode();

	void setEngineName(const QString& name);
	void setEngineVersion(const QString& version);
	void setSetup(const QString& setup);
	void setUnSetup(const QString& unSetup);
	void setInfo(const QString& info);

	QString getEngineName() const;
	QString getEngineVersion() const;
	QString getLocation() const;
	QString getSetup() const;
	QString getUnSetup() const;
	QString getInfo() const;

private Q_SLOTS:
	void selectLocation();
	void submit();

private:
	Ui::SubmitEngineDialog ui_;
	bool editMode_;
};

#endif // SUBMITENGINEDIALOG_HEADER_

