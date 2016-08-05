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
	ui_.showButton->setMenu(menu);

	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.showButton, &QPushButton::clicked, this, &EngineItemWidget::onShow);
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

void EngineItemWidget::onShow()
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

