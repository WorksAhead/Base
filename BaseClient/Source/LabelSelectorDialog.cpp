#include "LabelSelectorDialog.h"
#include <QAbstractButton>

LabelSelectorDialog::LabelSelectorDialog(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui_.setupUi(this);

	QObject::connect(ui_.buttonBox, &QDialogButtonBox::clicked, [this](QAbstractButton* button){
		switch (ui_.buttonBox->buttonRole(button))
		{
		case QDialogButtonBox::ResetRole:
			ui_.widget->setSelectedLabels(QStringList());
			break;
		case QDialogButtonBox::AcceptRole:
			accept();
			break;
		case QDialogButtonBox::RejectRole:
			reject();
			break;
		};
	});
}

LabelSelectorDialog::~LabelSelectorDialog()
{
}

LabelSelectorWidget* LabelSelectorDialog::labelSelectorWidget()
{
	return ui_.widget;
}

