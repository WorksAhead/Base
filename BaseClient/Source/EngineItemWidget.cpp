#include "EngineItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QProcess>
#include <QDesktopServices>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

#include <windows.h>

namespace fs = boost::filesystem;

EngineItemWidget::EngineItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QMenu* menu = new QMenu;
	ui_.browseButton->setMenu(menu);

	QAction* setupAction = menu->addAction("Setup");
	QAction* unSetupAction = menu->addAction("UnSetup");
	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.browseButton, &QPushButton::clicked, this, &EngineItemWidget::onBrowse);
	QObject::connect(setupAction, &QAction::triggered, this, &EngineItemWidget::onSetup);
	QObject::connect(unSetupAction, &QAction::triggered, this, &EngineItemWidget::onUnSetup);
	QObject::connect(removeAction, &QAction::triggered, this, &EngineItemWidget::onRemove);
}

EngineItemWidget::~EngineItemWidget()
{
}

void EngineItemWidget::setEngineVersion(const QPair<QString, QString>& v)
{
	engineVersion_ = v;
	ui_.nameLabel->setText(v.first + " " + v.second);
}

QPair<QString, QString> EngineItemWidget::getEngineVersion()
{
	return engineVersion_;
}

void EngineItemWidget::mousePressEvent(QMouseEvent*)
{
}

void EngineItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
}

void EngineItemWidget::resizeEvent(QResizeEvent*)
{
}

void EngineItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void EngineItemWidget::onSetup()
{
	context_->setupEngine(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));
}

void EngineItemWidget::onUnSetup()
{
	context_->unSetupEngine(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));
}

void EngineItemWidget::onBrowse()
{
	std::string path = context_->enginePath(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));
	QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromLocal8Bit(path.c_str())));
}

void EngineItemWidget::onRemove()
{
	QMessageBox mb(QMessageBox::Question, "Base",
		tr("Are you sure you want to remove this Engine ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes|QMessageBox::No);

	mb.setDefaultButton(QMessageBox::No);

	int ret = mb.exec();
	if (ret != QMessageBox::Yes) {
		return;
	}

	context_->removeEngine(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));
}

