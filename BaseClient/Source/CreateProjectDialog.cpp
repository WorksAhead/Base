#include "CreateProjectDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

CreateProjectDialog::CreateProjectDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui_.setupUi(this);

	QObject::connect(ui_.selectButton, &QPushButton::clicked, this, &CreateProjectDialog::onSelect);
	QObject::connect(ui_.createButton, &QPushButton::clicked, this, &CreateProjectDialog::onCreate);
}

CreateProjectDialog::~CreateProjectDialog()
{
}

void CreateProjectDialog::setDirectory(const QString& dir)
{
	ui_.dirEdit->setText(dir);
}

QString CreateProjectDialog::location() const
{
	fs::path location = ui_.locationEdit->text().toLocal8Bit().data();
	if (ui_.createDirCheckBox->isChecked()) {
		location /= ui_.dirEdit->text().toLocal8Bit().data();
	}
	return QString::fromLocal8Bit(location.string().c_str());
}

void CreateProjectDialog::onSelect()
{
	QString location = QFileDialog::getExistingDirectory(this, "Select Location");
	if (!location.isEmpty()) {
		fs::path p = location.toLocal8Bit().data();
		p.make_preferred();
		ui_.locationEdit->setText(QString::fromLocal8Bit(p.string().c_str()));
	}
}

void CreateProjectDialog::onCreate()
{
	if (ui_.locationEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Location field cannot be left empty."));
		return;
	}

	QFileInfo fileInfo(ui_.locationEdit->text());
	if (!fileInfo.exists() || !fileInfo.isDir() || fileInfo.isRoot()) {
		QMessageBox::information(this, "Base", tr("The specified location is not valid"));
		return;
	}

	if (ui_.createDirCheckBox->isChecked())
	{
		if (ui_.dirEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("Please enter a valid directory name."));
			return;
		}

		if (ui_.dirEdit->text().count(QRegularExpression(R"([\/:*?"<>|])"))) {
			QMessageBox::information(this, "Base", tr("Directory name cannot contain any of the following characters:\n" R"(\ / : * ? " < > |)"));
			return;
		}

		QFileInfo fileInfo(location());
		if (fileInfo.exists()) {
			QMessageBox::information(this, "Base", tr("The specified directory already exists."));
			return;
		}
	}

	done(1);
}

