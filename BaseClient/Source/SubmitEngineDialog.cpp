#include "SubmitEngineDialog.h"

#include <QFileDialog>
#include <QRegExpValidator>

SubmitEngineDialog::SubmitEngineDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QRegExp engineExp(R"([ A-Za-z0-9]*)");
	QRegExp versionExp(R"([ ,.A-Za-z0-9]*)");

	ui_.engineEdit->setValidator(new QRegExpValidator(engineExp));
	ui_.versionEdit->setValidator(new QRegExpValidator(versionExp));

	QObject::connect(ui_.selectPathButton, &QPushButton::clicked, this, &SubmitEngineDialog::selectPath);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitEngineDialog::submit);
}

SubmitEngineDialog::~SubmitEngineDialog()
{
}

void SubmitEngineDialog::selectPath()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Path");
	if (!path.isEmpty()) {
		ui_.pathEdit->setText(path);
	}
}

void SubmitEngineDialog::submit()
{
	engine_ = ui_.engineEdit->text();
	version_ = ui_.versionEdit->text();
	path_ = ui_.pathEdit->text();
	info_ = ui_.infoEdit->toPlainText();
	done(1);
}

