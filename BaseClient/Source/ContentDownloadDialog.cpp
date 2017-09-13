#include "ContentDownloadDialog.h"

#include <QMouseEvent>

ContentDownloadDialog::ContentDownloadDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.downloadButton, &QPushButton::clicked, this, &ContentDownloadDialog::onDownload);
}

ContentDownloadDialog::~ContentDownloadDialog()
{
}

void ContentDownloadDialog::setEngineVersionAboutToBeDownloaded(const QString& name, const QString& version)
{
	ui_.installEngineCheckBox->setText("Install Engine " + name + " " + version);
}

bool ContentDownloadDialog::isInstallEngineChecked()
{
	return ui_.installEngineCheckBox->isChecked();
}

void ContentDownloadDialog::onDownload()
{
	done(1);
}

