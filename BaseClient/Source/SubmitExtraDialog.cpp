#include "SubmitExtraDialog.h"

#include <QFileDialog>
#include <QRegExpValidator>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

SubmitExtraDialog::SubmitExtraDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QMenu* menu = new QMenu;

	ui_.macroButton->setMenu(menu);

	QAction* addExtraDir = menu->addAction("$(ExtraDir)");

	QObject::connect(addExtraDir, &QAction::triggered, [this](){
		ui_.setupEdit->insert("$(ExtraDir)");
	});

	QObject::connect(ui_.selectLocationButton, &QPushButton::clicked, this, &SubmitExtraDialog::selectLocation);
	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitExtraDialog::submit);

	editMode_ = false;
}

SubmitExtraDialog::~SubmitExtraDialog()
{
}

void SubmitExtraDialog::switchToEditMode()
{
	setWindowTitle(tr("Edit Extra"));
	ui_.locationEdit->setDisabled(true);
	ui_.selectLocationButton->setDisabled(true);
	editMode_ = true;
}

void SubmitExtraDialog::setTitle(const QString& title)
{
	ui_.titleEdit->setText(title);
}

void SubmitExtraDialog::setSetup(const QString& setup)
{
	ui_.setupEdit->setText(setup);
}


void SubmitExtraDialog::setInfo(const QString& info)
{
	ui_.infoEdit->setPlainText(info);
}

QString SubmitExtraDialog::getTitle() const
{
	return ui_.titleEdit->text();
}

QString SubmitExtraDialog::getLocation() const
{
	return ui_.locationEdit->text();
}

QString SubmitExtraDialog::getSetup() const
{
	return ui_.setupEdit->text();
}

QString SubmitExtraDialog::getInfo() const
{
	return ui_.infoEdit->toPlainText();
}

void SubmitExtraDialog::selectLocation()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Location");
	if (!path.isEmpty()) {
		fs::path p = path.toLocal8Bit().data();
		p.make_preferred();
		ui_.locationEdit->setText(QString::fromLocal8Bit(p.string().c_str()));
	}
}

void SubmitExtraDialog::submit()
{
	if (!editMode_)
	{
		if (ui_.titleEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Title field cannot be left empty."));
			return;
		}

		if (ui_.locationEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Location field cannot be left empty."));
			return;
		}

		QFileInfo fileInfo(ui_.locationEdit->text());

		if (!fileInfo.exists() || !fileInfo.isDir() || fileInfo.isRoot()) {
			QMessageBox::information(this, "Base", tr("\"%1\" is not a valid location."));
			return;
		}

		if (ui_.setupEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Setup field cannot be left empty."));
			return;
		}
	}

	done(1);
}

