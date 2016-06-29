#include "ImageCropperDialog.h"

ImageCropperDialog::ImageCropperDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);
}

ImageCropperDialog::~ImageCropperDialog()
{
}

ImageCropperWidget* ImageCropperDialog::imageCropperWidget()
{
	return ui_.imageCropper;
}

