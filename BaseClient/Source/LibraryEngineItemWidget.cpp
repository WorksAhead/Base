#include "LibraryEngineItemWidget.h"
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

LibraryEngineItemWidget::LibraryEngineItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QMenu* menu = new QMenu;
	ui_.browseButton->setMenu(menu);

	QAction* setupAction = menu->addAction("Setup");
	QAction* unSetupAction = menu->addAction("UnSetup");
	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.browseButton, &QPushButton::clicked, this, &LibraryEngineItemWidget::onBrowse);
	QObject::connect(setupAction, &QAction::triggered, this, &LibraryEngineItemWidget::onSetup);
	QObject::connect(unSetupAction, &QAction::triggered, this, &LibraryEngineItemWidget::onUnSetup);
	QObject::connect(removeAction, &QAction::triggered, this, &LibraryEngineItemWidget::onRemove);
}

LibraryEngineItemWidget::~LibraryEngineItemWidget()
{
}

void LibraryEngineItemWidget::setEngineVersion(const QPair<QString, QString>& v)
{
	engineVersion_ = v;
	ui_.nameLabel->setText(v.first + " " + v.second);
}

QPair<QString, QString> LibraryEngineItemWidget::getEngineVersion()
{
	return engineVersion_;
}

void LibraryEngineItemWidget::mousePressEvent(QMouseEvent*)
{
}

void LibraryEngineItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
}

void LibraryEngineItemWidget::resizeEvent(QResizeEvent*)
{
}

void LibraryEngineItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryEngineItemWidget::onSetup()
{
	EngineVersion v(engineVersion_.first.toStdString(), engineVersion_.second.toStdString());

	int state = EngineState::installed;
	if (!context_->changeEngineState(v, state, EngineState::configuring)) {
		context_->promptEngineState(v, state);
		return;
	}

	context_->setupEngine(v);

	state = EngineState::configuring;
	context_->changeEngineState(v, state, EngineState::installed);
}

void LibraryEngineItemWidget::onUnSetup()
{
	EngineVersion v(engineVersion_.first.toStdString(), engineVersion_.second.toStdString());

	int state = EngineState::installed;
	if (!context_->changeEngineState(v, state, EngineState::configuring)) {
		context_->promptEngineState(v, state);
		return;
	}

	context_->unSetupEngine(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));

	state = EngineState::configuring;
	context_->changeEngineState(v, state, EngineState::installed);
}

void LibraryEngineItemWidget::onBrowse()
{
	std::string path = context_->enginePath(EngineVersion(engineVersion_.first.toStdString(), engineVersion_.second.toStdString()));
	QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromLocal8Bit(path.c_str())));
}

void LibraryEngineItemWidget::onRemove()
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

