#ifndef IMAGECROPPERDIALOG_HEADER_
#define IMAGECROPPERDIALOG_HEADER_

#include "ui_ImageCropperDialog.h"

class ImageCropperDialog : public QDialog {
public:
	explicit ImageCropperDialog(QWidget* parent = 0);
	~ImageCropperDialog();

	ImageCropperWidget* imageCropperWidget();

private:
	Ui::ImageCropperDialog ui_;
};

#endif // IMAGECROPPERDIALOG_HEADER_

