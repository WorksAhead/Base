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
}

SubmitEngineDialog::~SubmitEngineDialog()
{
}

void SubmitEngineDialog::selectLocation()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Path");
	if (!path.isEmpty()) {
		fs::path p = path.toLocal8Bit().data();
		p.make_preferred();
		ui_.locationEdit->setText(QString::fromLocal8Bit(p.string().c_str()));
	}
}

void SubmitEngineDialog::submit()
{
	engineName_ = ui_.nameEdit->text();
	engineVersion_ = ui_.versionEdit->text();
	location_ = ui_.locationEdit->text();
	info_ = ui_.infoEdit->toPlainText();

	if (engineName_.isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Engine field cannot be left empty."));
		return;
	}

	if (engineVersion_.isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Version field cannot be left empty."));
		return;
	}

	if (location_.isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Path field cannot be left empty."));
		return;
	}

	QFileInfo fileInfo(location_);

	if (!fileInfo.exists() ||!fileInfo.isDir() || fileInfo.isRoot()) {
		QMessageBox::information(this, "Base", tr("\"%1\" is not a valid path."));
		return;
	}

	done(1);
}
