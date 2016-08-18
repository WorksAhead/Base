#include "SubmitEngineDialog.h"

#include <QFileDialog>
#include <QRegExpValidator>
#include <QFileInfo>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

SubmitEngineDialog::SubmitEngineDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QRegExp engineExp(R"([ A-Za-z0-9]*)");
	QRegExp versionExp(R"([ ,.A-Za-z0-9]*)");

	ui_.nameEdit->setValidator(new QRegExpValidator(engineExp));
	ui_.versionEdit->setValidator(new QRegExpValidator(versionExp));

	QObject::connect(ui_.selectLocationButton, &QPushButton::clicked, this, &SubmitEngineDialog::selectLocation);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitEngineDialog::submit);

	editMode_ = false;
}

SubmitEngineDialog::~SubmitEngineDialog()
{
}

void SubmitEngineDialog::switchToEditMode()
{
	setWindowTitle(tr("Edit Engine"));
	ui_.nameEdit->setReadOnly(true);
	ui_.versionEdit->setReadOnly(true);
	ui_.locationEdit->setDisabled(true);
	ui_.selectLocationButton->setDisabled(true);
	editMode_ = true;
}

void SubmitEngineDialog::setEngineName(const QString& name)
{
	ui_.nameEdit->setText(name);
}

void SubmitEngineDialog::setEngineVersion(const QString& version)
{
	ui_.versionEdit->setText(version);
}

void SubmitEngineDialog::setSetup(const QString& setup)
{
	ui_.setupEdit->setText(setup);
}

void SubmitEngineDialog::setUnSetup(const QString& unSetup)
{
	ui_.unSetupEdit->setText(unSetup);
}

void SubmitEngineDialog::setInfo(const QString& info)
{
	ui_.infoEdit->setPlainText(info);
}

QString SubmitEngineDialog::getEngineName() const
{
	return ui_.nameEdit->text();
}

QString SubmitEngineDialog::getEngineVersion() const
{
	return ui_.versionEdit->text();
}

QString SubmitEngineDialog::getLocation() const
{
	return ui_.locationEdit->text();
}

QString SubmitEngineDialog::getSetup() const
{
	return ui_.setupEdit->text();
}

QString SubmitEngineDialog::getUnSetup() const
{
	return ui_.unSetupEdit->text();
}

QString SubmitEngineDialog::getInfo() const
{
	return ui_.infoEdit->toPlainText();
}

void SubmitEngineDialog::selectLocation()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Location");
	if (!path.isEmpty()) {
		fs::path p = path.toLocal8Bit().data();
		p.make_preferred();
		ui_.locationEdit->setText(QString::fromLocal8Bit(p.string().c_str()));
	}
}

void SubmitEngineDialog::submit()
{
	if (!editMode_)
	{
		if (ui_.nameEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Engine field cannot be left empty."));
			return;
		}

		if (ui_.versionEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Version field cannot be left empty."));
			return;
		}

		if (ui_.locationEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Path field cannot be left empty."));
			return;
		}

		QFileInfo fileInfo(ui_.locationEdit->text());

		if (!fileInfo.exists() || !fileInfo.isDir() || fileInfo.isRoot()) {
			QMessageBox::information(this, "Base", tr("\"%1\" is not a valid location."));
			return;
		}
	}

	done(1);
}

