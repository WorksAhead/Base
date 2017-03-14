#include "SubmitExtraDialog.h"
#include "ImageCropperDialog.h"
#include "LabelSelectorDialog.h"

#include <QFileDialog>
#include <QRegExpValidator>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

SubmitExtraDialog::SubmitExtraDialog(ContextPtr context, QWidget* parent) : context_(context), QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	QMenu* menu = new QMenu;

	ui_.macroButton->setMenu(menu);

	QAction* addExtraDir = menu->addAction("$(ExtraDir)");

	QObject::connect(addExtraDir, &QAction::triggered, [this](){
		ui_.setupEdit->insert("$(ExtraDir)");
	});

	QObject::connect(ui_.editCategoryButton, &QPushButton::clicked, this, &SubmitExtraDialog::editCategory);
	QObject::connect(ui_.selectLocationButton, &QPushButton::clicked, this, &SubmitExtraDialog::selectLocation);
	QObject::connect(ui_.setCoverButton, &QPushButton::clicked, this, &SubmitExtraDialog::setCover);
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
	ui_.coverLabel->setDisabled(true);
	ui_.coverViewer->setDisabled(true);
	ui_.setCoverButton->setDisabled(true);
	editMode_ = true;
}

void SubmitExtraDialog::setTitle(const QString& title)
{
	ui_.titleEdit->setText(title);
}

void SubmitExtraDialog::setCategory(const QString& category)
{
	ui_.categoryEdit->setText(category);
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

QString SubmitExtraDialog::getCategory() const
{
	return ui_.categoryEdit->text();
}

QString SubmitExtraDialog::getSetup() const
{
	return ui_.setupEdit->text();
}

QString SubmitExtraDialog::getCoverImage() const
{
	return coverImage_;
}

QString SubmitExtraDialog::getInfo() const
{
	return ui_.infoEdit->toPlainText();
}

void SubmitExtraDialog::editCategory()
{
	Rpc::StringSeq categories;
	Rpc::ErrorCode ec = context_->session->getExtraCategories(categories);
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

void SubmitExtraDialog::selectLocation()
{
	QString path = QFileDialog::getExistingDirectory(this, "Select Location");
	if (!path.isEmpty()) {
		fs::path p = path.toLocal8Bit().data();
		p.make_preferred();
		ui_.locationEdit->setText(QString::fromLocal8Bit(p.string().c_str()));
	}
}

void SubmitExtraDialog::setCover()
{
	QPixmap pixmap = getImage(QSize(1, 1));
	if (pixmap.isNull()) {
		return;
	}

	ui_.coverViewer->setPixmap(pixmap);
}

void SubmitExtraDialog::submit()
{
	if (ui_.titleEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Title field cannot be left empty."));
		return;
	}

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
	}

	if (ui_.categoryEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Category field cannot be left empty."));
		return;
	}

	if (ui_.setupEdit->text().isEmpty()) {
		QMessageBox::information(this, "Base", tr("The Setup field cannot be left empty."));
		return;
	}

	if (!editMode_)
	{
		if (!ui_.coverViewer->pixmap()) {
			QMessageBox::information(this, "Base", tr("The Cover is not set."));
			return;
		}

		std::string imageFilename = context_->uniquePath() + ".jpg";

		if (!ui_.coverViewer->pixmap().scaled(QSize(300, 300), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(imageFilename.c_str(), "JPG", 90)) {
			QMessageBox::information(this, "Base", tr("Failed to save image"));
			return;
		}

		coverImage_ = imageFilename.c_str();
	}

	done(1);
}

QPixmap SubmitExtraDialog::getImage(const QSize& ratio)
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

QString SubmitExtraDialog::getOpenImageFileName()
{
	return QFileDialog::getOpenFileName(this, "Open File", QString(), "Images (*.bmp *.gif *.png *.jpg *.jpeg *.pbm *.pgm *.ppm)");
}

