#include "SubmitClientDialog.h"

#include <QFileDialog>
#include <QRegExp>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>

#include <boost/filesystem.hpp>

#include <string>
#include <fstream>

namespace fs = boost::filesystem;

SubmitClientDialog::SubmitClientDialog(ContextPtr context, QWidget* parent) : context_(context), QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.selectLocationButton, &QPushButton::clicked, this, &SubmitClientDialog::selectLocation);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitClientDialog::submit);

	editMode_ = false;
}

SubmitClientDialog::~SubmitClientDialog()
{
}

void SubmitClientDialog::switchToEditMode()
{
	setWindowTitle(tr("Edit Client"));
	ui_.versionEdit->setDisabled(true);
	ui_.locationEdit->setDisabled(true);
	ui_.selectLocationButton->setDisabled(true);
	editMode_ = true;
}

void SubmitClientDialog::setVersion(const QString& version)
{
	ui_.versionEdit->setText(version);
}

void SubmitClientDialog::setInfo(const QString& info)
{
	ui_.infoEdit->setPlainText(info);
}

QString SubmitClientDialog::getLocation() const
{
	return ui_.locationEdit->text();
}

QString SubmitClientDialog::getVersion() const
{
	return ui_.versionEdit->text();
}

QString SubmitClientDialog::getInfo() const
{
	return ui_.infoEdit->toPlainText();
}

void SubmitClientDialog::selectLocation()
{
	QString location = QFileDialog::getExistingDirectory(this, "Select Location");

	if (!location.isEmpty())
	{
		fs::path path = location.toLocal8Bit().data();

		fs::path workDir = path / "Bin";
		fs::path program = workDir / "BaseClient.exe";

		QProcess p;

		p.setWorkingDirectory(QString::fromLocal8Bit(workDir.string().c_str()));
		p.start(program.string().c_str(), QStringList() << "ver");

		if (!p.waitForStarted(-1)) {
			context_->prompt(0, "Failed to get Client version information from specified location.");
			return;
		}

		if (!p.waitForFinished(1000)) {
			return;
		}

		if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) {
			context_->prompt(1, "Client application fault.");
			return;
		}

		std::string version;

		std::fstream is((path / "version").string().c_str(), std::ios::in);
		if (!is.is_open() || !std::getline(is, version)) {
			context_->prompt(1, "Client version information not exists.");
			return;
		}

		is.close();

		QRegExp re(R"(\d+\.\d+\.\d+\.\d+)");

		if (!re.exactMatch(version.c_str())) {
			context_->prompt(1, "Bad version information.");
			return;
		}

		ui_.versionEdit->setText(version.c_str());
		ui_.locationEdit->setText(location);
	}
}

void SubmitClientDialog::submit()
{
	if (!editMode_)
	{
		if (ui_.locationEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Location field cannot be left empty."));
			return;
		}

		QFileInfo fileInfo(ui_.locationEdit->text());

		if (!fileInfo.exists() || !fileInfo.isDir() || fileInfo.isRoot()) {
			QMessageBox::information(this, "Base", tr("\"%1\" is not a valid location."));
			return;
		}

		if (ui_.versionEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Version field cannot be left empty."));
			return;
		}
	}

	done(1);
}

