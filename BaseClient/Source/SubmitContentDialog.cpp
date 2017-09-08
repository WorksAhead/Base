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
#include <QInputDialog>
#include <QRegExpValidator>
#include <QShortcut>

#include <boost/filesystem.hpp>

#include <memory>
#include <math.h>

#define VERSION_FORMAT R"((0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*))"

namespace fs = boost::filesystem;

SubmitContentDialog::SubmitContentDialog(ContextPtr context, QWidget* parent) : QDialog(parent), context_(context)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.versionEdit->setValidator(new QRegExpValidator(QRegExp(VERSION_FORMAT)));

	ui_.summaryEdit->setPlaceholderText("Summary");
	ui_.descriptionEdit->setPlaceholderText("Description");

	QObject::connect(ui_.editPageButton, &QPushButton::clicked, this, &SubmitContentDialog::onEditPage);
	QObject::connect(ui_.editCategoryButton, &QPushButton::clicked, this, &SubmitContentDialog::onEditCategory);

	QObject::connect(ui_.addEngineVersionButton, &QPushButton::clicked, this, &SubmitContentDialog::onAddEngineVersion);
	QObject::connect(ui_.removeEngineVersionButton, &QPushButton::clicked, this, &SubmitContentDialog::onRemoveEngineVersion);

	QMenu* menu1 = new QMenu;
	QMenu* menu2 = new QMenu;

	ui_.macroButton1->setMenu(menu1);
	ui_.macroButton2->setMenu(menu2);

	QAction* addEngineDir1 = menu1->addAction("$(EngineDir)");
	QAction* addProjectDir1 = menu1->addAction("$(ProjectDir)");

	QAction* addEngineDir2 = menu2->addAction("$(EngineDir)");
	QAction* addProjectDir2 = menu2->addAction("$(ProjectDir)");

	QObject::connect(ui_.browseLocationButton, &QPushButton::clicked, this, &SubmitContentDialog::onBrowseLocation);
	QObject::connect(ui_.browseProjectLocationButton, &QPushButton::clicked, this, &SubmitContentDialog::onBrowseProjectLocation);

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

	QObject::connect(ui_.projectInSubDirCheckBox, &QCheckBox::toggled, [this](bool checked)
	{
		if (!checked) {
			ui_.projectLocationEdit->clear();
		}
	});

	QObject::connect(ui_.engineVersionEdit, &QLineEdit::textChanged, [this]()
	{
		generateCommandAndWorkDir();
	});

	QObject::connect(ui_.locationEdit, &QLineEdit::textChanged, [this]()
	{
		generateCommandAndWorkDir();
	});

	QObject::connect(ui_.projectLocationEdit, &QLineEdit::textChanged, [this]()
	{
		generateCommandAndWorkDir();
	});

	QObject::connect(ui_.setCoverButton, &QPushButton::clicked, this, &SubmitContentDialog::onSetCover);

	QObject::connect(ui_.prevScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onPrevScreenshot);
	QObject::connect(ui_.nextScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onNextScreenshot);
	QObject::connect(ui_.addScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onAddScreenshot);
	QObject::connect(ui_.removeScreenshotButton, &QPushButton::clicked, this, &SubmitContentDialog::onRemoveScreenshot);

	QObject::connect(ui_.submitButton, &QPushButton::clicked, this, &SubmitContentDialog::onSubmit);

	QShortcut* showAllShortcut = new QShortcut(QKeySequence(Qt::Key_F12), this);

	QObject::connect(showAllShortcut, &QShortcut::activated, this, &SubmitContentDialog::onShowAll);

	ui_.prevScreenshotButton->setEnabled(false);
	ui_.nextScreenshotButton->setEnabled(false);
	ui_.removeScreenshotButton->setEnabled(false);

	ui_.parentIdEdit->setVisible(false);
	ui_.commandEdit->setVisible(false);
	ui_.macroButton1->setVisible(false);
	ui_.workDirEdit->setVisible(false);
	ui_.macroButton2->setVisible(false);

	editMode_ = false;
	copyMode_ = false;

	script_.loadFromServer(context_, "SubmitContent.lua");
}

SubmitContentDialog::~SubmitContentDialog()
{
}

void SubmitContentDialog::setEngineVersions(const QStringList& engineVersions)
{
	engineVersions_ = engineVersions;
}

void SubmitContentDialog::switchToEditMode(const QString& contentId)
{
	setWindowTitle(tr("Edit Content"));

	ui_.locationEdit->setEnabled(false);
	ui_.browseLocationButton->setEnabled(false);
	ui_.projectInSubDirCheckBox->setEnabled(false);
	ui_.coverLabel->setEnabled(false);
	ui_.coverViewer->setEnabled(false);
	ui_.setCoverButton->setEnabled(false);
	ui_.screenshotLabel->setEnabled(false);
	ui_.screenshotWidget->setEnabled(false);
	ui_.addScreenshotButton->setEnabled(false);
	ui_.removeScreenshotButton->setEnabled(false);
	ui_.prevScreenshotButton->setEnabled(false);
	ui_.nextScreenshotButton->setEnabled(false);

	onShowAll();

	contentId_ = contentId;
	editMode_ = true;
}

void SubmitContentDialog::switchToCopyMode(const QString& contentId)
{
	setWindowTitle(tr("Copy Content"));

	ui_.locationEdit->setEnabled(false);
	ui_.browseLocationButton->setEnabled(false);
	ui_.projectInSubDirCheckBox->setEnabled(false);

	contentId_ = contentId;
	copyMode_ = true;
}

void SubmitContentDialog::loadImagesFrom(const QString& contentId, int count)
{
	QObject::connect(context_->contentImageLoader, &ContentImageLoader::loaded, this, &SubmitContentDialog::onImageLoaded);

	setProperty("image id", contentId);

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
	QStringList v = title.split('\r');

	ui_.nameEdit->setText((v.count() > 0) ? v[0] : "");
	ui_.versionEdit->setText((v.count() > 1) ? v[1] : "0.0.0");
}

void SubmitContentDialog::setPage(const QString& name)
{
	ui_.pageEdit->setText(name);
}

void SubmitContentDialog::setCategory(const QString& category)
{
	ui_.categoryEdit->setText(category);
}

void SubmitContentDialog::setEngineNameAndVersion(const QString& name, const QString& version)
{
	if (name.isEmpty()) {
		ui_.engineVersionEdit->setText("N/A\r");
	}
	else {
		ui_.engineVersionEdit->setText(name + "\r" + version);
	}
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
	std::string d = desc.toStdString();

	if (boost::istarts_with(d, "<!SUMMARY>"))
	{
		size_t p = d.find("<!DESCRIPTION>", 10);

		if (p != std::string::npos) {
			ui_.summaryEdit->setHtml(d.substr(10, p - 10).c_str());
			ui_.descriptionEdit->setHtml(d.substr(p + 14).c_str());
		}
		else {
			ui_.summaryEdit->setHtml(d.substr(10).c_str());
		}
	}
	else if (boost::istarts_with(d, "<!DOCTYPE HTML"))
	{
		ui_.summaryEdit->setHtml(d.c_str());
	}
	else
	{
		ui_.summaryEdit->setPlainText(d.c_str());
	}
}

void SubmitContentDialog::onShowAll()
{
	ui_.parentIdEdit->setVisible(true);
	ui_.commandEdit->setVisible(true);
	ui_.macroButton1->setVisible(true);
	ui_.workDirEdit->setVisible(true);
	ui_.macroButton2->setVisible(true);
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
	Rpc::ErrorCode ec = context_->session->getContentCategories(categories);
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

void SubmitContentDialog::onAddEngineVersion()
{
	QString name;
	QStringList version;
	{
		QStringList list = ui_.engineVersionEdit->text().split('\r');

		if (list.count() == 2)
		{
			name = list[0];
			version = list[1].split('|');
		}
	}

	QStringList items;

	for (QString v : engineVersions_)
	{
		QStringList vv = v.split('\r');

		if (!name.isEmpty() && vv[0] != name) {
			continue;
		}
		else if (version.contains(vv[1])) {
			continue;
		}

		items.append(v);
	}

	if (name == "N/A") {
		return;
	}
	else if (name.isEmpty()) {
		items.push_front("N/A\r");
	}
	else if (items.isEmpty()) {
		return;
	}

	bool ok;
	QString item = QInputDialog::getItem(this, tr("Add Engine Version"), tr("Engine Version"), items, 0, false, &ok, Qt::WindowTitleHint);

	if (ok && !item.isEmpty())
	{
		QStringList v = item.split('\r');

		name = v[0];
		version.append(v[1]);

		QString text = name + "\r";

		for (int i = 0; i < version.count(); ++i)
		{
			if (i != 0) {
				text += "|";
			}

			text += version[i];
		}

		ui_.engineVersionEdit->setText(text);
	}
}

void SubmitContentDialog::onRemoveEngineVersion()
{
	QString name;
	QStringList version;
	{
		QStringList list = ui_.engineVersionEdit->text().split('\r');

		if (list.count() == 2)
		{
			name = list[0];
			version = list[1].split('|');
		}
	}

	if (!version.isEmpty())
	{
		version.pop_back();

		if (!version.isEmpty())
		{
			QString text = name + "\r";

			for (int i = 0; i < version.count(); ++i)
			{
				if (i != 0) {
					text += "|";
				}

				text += version[i];
			}

			ui_.engineVersionEdit->setText(text);
		}
		else
		{
			ui_.engineVersionEdit->clear();
		}
	}
}

void SubmitContentDialog::onBrowseLocation()
{
	QString s = QFileDialog::getExistingDirectory(this, "Select location");

	if (!s.isEmpty())
	{
		s.replace('/', '\\');

		if (s.endsWith('\\')) {
			s.remove(s.count() - 1, 1);
		}

		ui_.locationEdit->setText(s);
	}
}

void SubmitContentDialog::onBrowseProjectLocation()
{
	QString s = QFileDialog::getExistingDirectory(this, "Select project location", ui_.locationEdit->text());

	if (!s.isEmpty())
	{
		s.replace('/', '\\');

		if (s.endsWith('\\')) {
			s.remove(s.count() - 1, 1);
		}

		if (s.startsWith(ui_.locationEdit->text() + "\\", Qt::CaseInsensitive) && s.count() > ui_.locationEdit->text().count() + 1)
		{
			s.remove(0, ui_.locationEdit->text().count() + 1);
			ui_.projectLocationEdit->setText(s);
		}
		else
		{
			QMessageBox::information(this, "Base", tr("Project Location must be a sub-directory of Location."));
			return;
		}
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
		ec = context_->session->editContent(contentId_.toStdString(), submitter);
		CHECK_ERROR_CODE(ec);
	}
	else if (copyMode_) {
		ec = context_->session->copyContent(contentId_.toStdString(), submitter);
		CHECK_ERROR_CODE(ec);
	}
	else {
		ec = context_->session->submitContent(submitter);
		CHECK_ERROR_CODE(ec);
	}

	if (ui_.nameEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Name field cannot be left empty."));
		return;
	}
	else if (ui_.versionEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Version field cannot be left empty."));
		return;
	}

	if (!QRegExp(VERSION_FORMAT).exactMatch(ui_.versionEdit->text())) {
		QMessageBox::information(this, "Base", tr("Invalid Version format."));
		return;
	}

	ec = submitter->setTitle((ui_.nameEdit->text() + "\r" + ui_.versionEdit->text()).toStdString());
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

	if (ui_.engineVersionEdit->text().isEmpty())
	{
		QMessageBox::information(this, "Base", tr("The Engine Version field cannot be left empty."));
		return;
	}

	QString engineName, engineVersion;
	{
		QStringList list = ui_.engineVersionEdit->text().split('\r');

		engineName = list[0];
		engineVersion = list[1];

		if (engineName == "N/A")
		{
			engineName.clear();
			engineVersion.clear();
		}
	}

	ec = submitter->setEngine(engineName.toStdString(), engineVersion.toStdString());
	CHECK_ERROR_CODE(ec);

	if (!editMode_ && !copyMode_)
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

		if (!checkLocation()) {
			return;
		}
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

	if (ui_.summaryEdit->toPlainText().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Summary cannot be left empty."));
		return;
	}

	ec = submitter->setVideo(ui_.videoEdit->toPlainText().toStdString());
	CHECK_ERROR_CODE(ec);

	QString desc = "<!SUMMARY>" + ui_.summaryEdit->toHtml();

	if (!ui_.descriptionEdit->toPlainText().isEmpty()) {
		desc = desc + "<!DESCRIPTION>" + ui_.descriptionEdit->toHtml();
	}

	ec = submitter->setDescription(desc.toStdString());
	CHECK_ERROR_CODE(ec);

	if (!editMode_)
	{
		boost::shared_ptr<ASyncSubmitContentTask> task(new ASyncSubmitContentTask(context_, submitter));

		task->setInfoHead(QString("Submit %1 %2").arg(ui_.nameEdit->text()).arg(ui_.versionEdit->text()).toLocal8Bit().data());
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

void SubmitContentDialog::onImageLoaded(const QString& contentId, int index, const QPixmap& pixmap)
{
	if (property("image id").toString() == contentId)
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
}

void SubmitContentDialog::keyPressEvent(QKeyEvent* e)
{
	if (e->key() != Qt::Key_Escape) {
		QDialog::keyPressEvent(e);
	}
}

void SubmitContentDialog::generateCommandAndWorkDir()
{
	QStringList list = ui_.engineVersionEdit->text().split('\r');

	if (list.count() != 2)
	{
		ui_.commandEdit->clear();
		ui_.workDirEdit->clear();
		return;
	}

	std::string engineName = list[0].toStdString();
	std::string engineVersion = list[1].toStdString();
	std::string location = ui_.locationEdit->text().toStdString();
	std::string projectLocation = ui_.projectLocationEdit->text().toStdString();

	kaguya::State& state = script_.state();

	auto f = state["generateCommandAndWorkDir"];
	{
		std::string command;
		std::string workDir;

		kaguya::tie(command, workDir) = f(engineName, engineVersion, location, projectLocation);

		ui_.commandEdit->setText(command.c_str());
		ui_.workDirEdit->setText(workDir.c_str());
	}
}

bool SubmitContentDialog::checkLocation()
{
	QStringList list = ui_.engineVersionEdit->text().split('\r');

	if (list.count() != 2) {
		return false;
	}

	std::string engineName = list[0].toStdString();
	std::string engineVersion = list[1].toStdString();
	std::string location = ui_.locationEdit->text().toStdString();
	std::string projectLocation = ui_.projectLocationEdit->text().toStdString();

	kaguya::State& state = script_.state();

	auto f = state["checkLocation"];
	{
		bool valid = f(engineName, engineVersion, location, projectLocation);

		if (!valid)
		{
			QMessageBox msgBox(this);
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setWindowTitle("Base");
			msgBox.setText(tr("Location might be invalid"));

			QPushButton* b0 = msgBox.addButton("Back", QMessageBox::NoRole);
			QPushButton* b1 = msgBox.addButton("Continue anyway", QMessageBox::NoRole);

			msgBox.setDefaultButton(b0);
			msgBox.exec();

			if (msgBox.clickedButton() == b0) {
				return false;
			}
		}

		return true;
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

