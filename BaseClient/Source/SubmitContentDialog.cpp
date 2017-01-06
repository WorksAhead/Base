#include "SubmitContentDialog.h"
#include "LabelSelectorDialog.h"
#include "ImageCropperDialog.h"
#include "ImageViewerWidget.h"
#include "ASyncSubmitContentTask.h"
#include "ContentImageLoader.h"
#include "QtUtils.h"

#include <ErrorMessage.h>

#include <QFileDialog>
#include <QKeyEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <boost/filesystem.hpp>

#include <memory>
#include <math.h>

namespace fs = boost::filesystem;

SubmitContentDialog::SubmitContentDialog(ContextPtr context, QWidget* parent) : QDialog(parent), context_(context)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QObject::connect(ui_.editPageButton, &QPushButton::clicked, this, &SubmitContentDialog::onEditPage);
	QObject::connect(ui_.editCategoryButton, &QPushButton::clicked, this, &SubmitContentDialog::onEditCategory);

	QMenu* menu1 = new QMenu;
	QMenu* menu2 = new QMenu;

	ui_.macroButton1->setMenu(menu1);
	ui_.macroButton2->setMenu(menu2);

	QAction* addEngineDir1 = menu1->addAction("$(EngineDir)");
	QAction* addProjectDir1 = menu1->addAction("$(ProjectDir)");

	QAction* addEngineDir2 = menu2->addAction("$(EngineDir)");
	QAction* addProjectDir2 = menu2->addAction("$(ProjectDir)");

	QObject::connect(ui_.selectLocationButton, &QPushButton::clicked, this, &SubmitContentDialog::onSelectLocation);

	QObject::connect(addEngineDir1, &QAction::triggered, [this](){
		ui_.commandEdit->insert("$(EngineDir)");
	});
	QObject::connect(addProjectDir1, &QAction::triggered, [this](){
		ui_.commandEdit->insert("$(ProjectDir)");
	});
	QObject::connect(addEngineDir2, &QAction::triggered, [this](){
		ui_.workDirEdit->insert("$(EngineDir)");
	});
	QObject::connect(addProjectDir2, &QAction::triggered, [this](){
		ui_.workDirEdit->insert("$(ProjectDir)");
	});

	QObject::connect(ui_.setCoverButton, &QPushButton::clicked, this, &SubmitContentDialog::onSetCover);

	QObject::connect(ui_.prevScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onPrevScreenshot);
	QObject::connect(ui_.nextScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onNextScreenshot);
	QObject::connect(ui_.addScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onAddScreenshot);
	QObject::connect(ui_.removeScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onRemoveScreenshot);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitContentDialog::onSubmit);

	ui_.prevScreenshotButton->setEnabled(false);
	ui_.nextScreenshotButton->setEnabled(false);
	ui_.removeScreenshotButton->setEnabled(false);

	editMode_ = false;
}

SubmitContentDialog::~SubmitContentDialog()
{
}

void SubmitContentDialog::switchToEditMode(const QString& contentId)
{
	setWindowTitle(tr("Edit Content"));

	ui_.locationEdit->setEnabled(false);
	ui_.selectLocationButton->setEnabled(false);
	ui_.coverLabel->setEnabled(false);
	ui_.coverViewer->setEnabled(false);
	ui_.setCoverButton->setEnabled(false);
	ui_.screenshotLabel->setEnabled(false);
	ui_.screenshotWidget->setEnabled(false);
	ui_.addScreenshotButton->setEnabled(false);
	ui_.removeScreenshotButton->setEnabled(false);
	ui_.prevScreenshotButton->setEnabled(false);
	ui_.nextScreenshotButton->setEnabled(false);

	contentId_ = contentId;
	editMode_ = true;
}

void SubmitContentDialog::loadImagesFrom(const QString& contentId, int count)
{
	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &SubmitContentDialog::onImageLoaded);

	for (int i = 0; i < count; ++i) {
		context_->contentImageLoader->load(contentId, i);
	}
}

void SubmitContentDialog::setParentId(const QString& parentId)
{
	ui_.parentIdEdit->setText(parentId);
}

void SubmitContentDialog::setTitle(const QString& title)
{
	ui_.titleEdit->setText(title);
}

void SubmitContentDialog::setPage(const QString& name)
{
	ui_.pageEdit->setText(name);
}

void SubmitContentDialog::setCategory(const QString& category)
{
	ui_.categoryEdit->setText(category);
}

void SubmitContentDialog::setEngineName(const QString& name)
{
	ui_.engineNameEdit->setText(name);
}

void SubmitContentDialog::setEngineVersion(const QString& version)
{
	ui_.engineVersionEdit->setText(version);
}

void SubmitContentDialog::setCommand(const QString& command)
{
	ui_.commandEdit->setText(command);
}

void SubmitContentDialog::setWorkingDir(const QString& workDir)
{
	ui_.workDirEdit->setText(workDir);
}

void SubmitContentDialog::setVideo(const QString& video)
{
	ui_.videoEdit->setPlainText(video);
}

void SubmitContentDialog::setDesc(const QString& desc)
{
	ui_.descriptionEdit->setPlainText(desc);
}

void SubmitContentDialog::onEditPage()
{
	Rpc::StringSeq pages;
	Rpc::ErrorCode ec = context_->session->getPages(pages);
	if (ec != Rpc::ec_success) {
		return;
	}

	QStringList list;

	list << "=Content pages=";

	for (const std::string& s : pages) {
		if (!boost::ends_with(s, "*") && !boost::contains(s, ",")) {
			list << s.c_str();
		}
	}

	LabelSelectorDialog d;

	d.labelSelectorWidget()->setLabels(list);
	d.labelSelectorWidget()->setSelectedLabels(ui_.pageEdit->text().split(','));

	if (d.exec() == 0) {
		return;
	}

	list = d.labelSelectorWidget()->getSelectedLabels();

	QString text;

	for (const QString& s : list) {
		if (!text.isEmpty()) {
			text += ", ";
		}
		text += s;
	}

	ui_.pageEdit->setText(text);
}

void SubmitContentDialog::onEditCategory()
{
	Rpc::StringSeq categories;
	Rpc::ErrorCode ec = context_->session->getCategories(categories);
	if (ec != Rpc::ec_success) {
		return;
	}

	QStringList list;

	for (const std::string& s : categories) {
		list << s.c_str();
	}

	LabelSelectorDialog d;

	d.labelSelectorWidget()->setLabels(list);
	d.labelSelectorWidget()->setSelectedLabels(ui_.categoryEdit->text().split(','));

	if (d.exec() == 0) {
		return;
	}

	list = d.labelSelectorWidget()->getSelectedLabels();

	QString text;

	for (const QString& s : list) {
		if (!text.isEmpty()) {
			text += ", ";
		}
		text += s;
	}

	ui_.categoryEdit->setText(text);
}

void SubmitContentDialog::onSelectLocation()
{
	QString s = QFileDialog::getExistingDirectory(this, "Select location");

	if (!s.isEmpty()) {
		ui_.locationEdit->setText(s);
	}
}

void SubmitContentDialog::onSetCover()
{
	QPixmap pixmap = getImage(QSize(1, 1));
	if (pixmap.isNull()) {
		return;
	}

	ui_.coverViewer->setPixmap(pixmap);
}

void SubmitContentDialog::onPrevScreenshot()
{
	int index = ui_.screenshotWidget->currentIndex();
	if (index > 0) {
		ui_.screenshotWidget->setCurrentIndex(index - 1);
	}
}

void SubmitContentDialog::onNextScreenshot()
{
	int index = ui_.screenshotWidget->currentIndex();
	if (index < ui_.screenshotWidget->count() - 1) {
		ui_.screenshotWidget->setCurrentIndex(index + 1);
	}
}

void SubmitContentDialog::onAddScreenshot()
{
	QPixmap pixmap = getImage(QSize(16, 9));
	if (pixmap.isNull()) {
		return;
	}

	ImageViewerWidget* imageViewer = new ImageViewerWidget;
	imageViewer->setPixmap(pixmap);

	const int index = ui_.screenshotWidget->addWidget(imageViewer);
	ui_.screenshotWidget->setCurrentIndex(index);

	const int count = ui_.screenshotWidget->count();

	if (count > 1) {
		ui_.prevScreenshotButton->setEnabled(true);
		ui_.nextScreenshotButton->setEnabled(true);
	}

	if (count > 0) {
		ui_.removeScreenshotButton->setEnabled(true);
	}

	//if (count >= 5) {
	//	ui_.addScreenshotButton->setEnabled(false);
	//}
}

void SubmitContentDialog::onRemoveScreenshot()
{
	const int index = ui_.screenshotWidget->currentIndex();
	if (index >= 0) {
		QWidget* w = ui_.screenshotWidget->widget(index);
		ui_.screenshotWidget->removeWidget(w);
	}

	const int count = ui_.screenshotWidget->count();

	/*if (count < 5) {
		ui_.addScreenshotButton->setEnabled(true);
	}*/

	if (count <= 1) {
		ui_.prevScreenshotButton->setEnabled(false);
		ui_.nextScreenshotButton->setEnabled(false);
	}

	if (count <= 0) {		
		ui_.removeScreenshotButton->setEnabled(false);
	}
}

void SubmitContentDialog::onSubmit()
{
#define CHECK_ERROR_CODE(ec)										\
	if (ec != Rpc::ec_success) {									\
		QMessageBox::information(this, "Base", errorMessage(ec));	\
		return;														\
	}

	Rpc::ContentSubmitterPrx submitter;

	Rpc::ErrorCode ec;

	if (editMode_) {
		ec = context_->session->updateContent(contentId_.toStdString(), submitter);
		CHECK_ERROR_CODE(ec);
	}
	else {
		ec = context_->session->submitContent(submitter);
		CHECK_ERROR_CODE(ec);
	}

	if (ui_.titleEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Title field cannot be left empty."));
		return;
	}

	ec = submitter->setTitle(ui_.titleEdit->text().toStdString());
	CHECK_ERROR_CODE(ec);

	if (ui_.pageEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Page field cannot be left empty."));
		return;
	}

	ec = submitter->setPage(ui_.pageEdit->text().toStdString());
	CHECK_ERROR_CODE(ec);

	if (ui_.categoryEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Category field cannot be left empty."));
		return;
	}

	ec = submitter->setCategory(ui_.categoryEdit->text().toStdString());
	CHECK_ERROR_CODE(ec);

	if (ui_.engineNameEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Engine Name field cannot be left empty."));
		return;
	}
	else if (ui_.engineVersionEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Engine Version field cannot be left empty."));
		return;
	}

	ec = submitter->setEngine(ui_.engineNameEdit->text().toStdString(), ui_.engineVersionEdit->text().toStdString());
	CHECK_ERROR_CODE(ec);

	if (!editMode_)
	{
		if (ui_.locationEdit->text().isEmpty()) {
			QMessageBox::information(this, "Base", tr("The Location is not specified."));
			return;
		}

		QFileInfo fileInfo(ui_.locationEdit->text());

		if (!fileInfo.exists() || !fileInfo.isDir() || fileInfo.isRoot()) {
			QMessageBox::information(this, "Base", tr("\"%1\" is not a valid location."));
			return;
		}
	}

	if (ui_.commandEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Command field cannot be left empty."));
		return;
	}

	QString startup = ui_.commandEdit->text();
	if (!ui_.workDirEdit->text().isEmpty()) {
		startup += "\n";
		startup += ui_.workDirEdit->text();
	}

	ec = submitter->setStartup(startup.toStdString());
	CHECK_ERROR_CODE(ec);

	if (!ui_.parentIdEdit->text().isEmpty()) {
		ec = submitter->setParentId(ui_.parentIdEdit->text().toStdString());
		CHECK_ERROR_CODE(ec);
	}

	if (!editMode_)
	{
		if (!ui_.coverViewer->pixmap()) {
			QMessageBox::information(this, "Base", tr("The Cover is not set."));
			return;
		}
	}

	if (ui_.descriptionEdit->toPlainText().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Description cannot be left empty."));
		return;
	}

	ec = submitter->setVideo(ui_.videoEdit->toPlainText().toStdString());
	CHECK_ERROR_CODE(ec);

	ec = submitter->setDescription(ui_.descriptionEdit->toPlainText().toStdString());
	CHECK_ERROR_CODE(ec);

	if (!editMode_)
	{
		boost::shared_ptr<ASyncSubmitContentTask> task(new ASyncSubmitContentTask(context_, submitter));

		task->setInfoHead(QString("Submit %1").arg(ui_.titleEdit->text()).toLocal8Bit().data());
		task->setContentLocation(ui_.locationEdit->text().toLocal8Bit().data());

		std::string imageFilename;

		imageFilename = context_->uniquePath() + ".jpg";

		if (!ui_.coverViewer->pixmap().scaled(QSize(300, 300), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(imageFilename.c_str(), "JPG", 90)) {
			QMessageBox::information(this, "Base", tr("Failed to save image"));
			return;
		}

		task->addImageFile(imageFilename);

		for (int i = 0; i < ui_.screenshotWidget->count(); ++i) {
			ImageViewerWidget* imageViewer = static_cast<ImageViewerWidget*>(ui_.screenshotWidget->widget(i));
			imageFilename = context_->uniquePath() + ".jpg";
			if (!imageViewer->pixmap().save(imageFilename.c_str(), "JPG", 90)) {
				QMessageBox::information(this, "Base", tr("Failed to save image"));
				return;
			}
			task->addImageFile(imageFilename);
		}

		context_->addTask(task);
	}
	else
	{
		ec = submitter->finish();
		context_->promptRpcError(ec);
	}

	done(1);

#undef CHECK_ERROR_CODE
}

void SubmitContentDialog::onImageLoaded(const QString&, int index, const QPixmap& pixmap)
{
	if (index == 0) {
		ui_.coverViewer->setPixmap(pixmap);
	}
	else if (index > 0)
	{
		ImageViewerWidget* imageViewer = new ImageViewerWidget;
		imageViewer->setPixmap(pixmap);

		ui_.screenshotWidget->insertWidget(index - 1, imageViewer);

		const int count = ui_.screenshotWidget->count();

		if (count > 1) {
			ui_.prevScreenshotButton->setEnabled(true);
			ui_.nextScreenshotButton->setEnabled(true);
		}

		if (count > 0) {
			ui_.removeScreenshotButton->setEnabled(true);
		}

		/*if (count >= 5) {
			ui_.addScreenshotButton->setEnabled(false);
		}*/
	}
}

void SubmitContentDialog::keyPressEvent(QKeyEvent* e)
{
	if (e->key() != Qt::Key_Escape) {
		QDialog::keyPressEvent(e);
	}
}

QPixmap SubmitContentDialog::getImage(const QSize& ratio)
{
	QString path = getOpenImageFileName();
	if (path.isEmpty()) {
		return QPixmap();
	}

	ImageCropperDialog d(this);

	d.imageCropperWidget()->setImageAspectRatio(ratio);
	d.imageCropperWidget()->setImage(QPixmap(path));

	int rc = d.exec();
	if (rc == 0) {
		return QPixmap();
	}

	return d.imageCropperWidget()->cropImage();
}

QString SubmitContentDialog::getOpenImageFileName()
{
	return QFileDialog::getOpenFileName(this, "Open File", QString(), "Images (*.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm)");
}
