#include "LibraryExtraItemWidget.h"

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

namespace fs = boost::filesystem;

LibraryExtraItemWidget::LibraryExtraItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QMenu* menu = new QMenu;
	ui_.browseButton->setMenu(menu);

	QAction* setupAction = menu->addAction("Setup");
	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.browseButton, &QPushButton::clicked, this, &LibraryExtraItemWidget::onBrowse);
	QObject::connect(setupAction, &QAction::triggered, this, &LibraryExtraItemWidget::onSetup);
	QObject::connect(removeAction, &QAction::triggered, this, &LibraryExtraItemWidget::onRemove);
}

LibraryExtraItemWidget::~LibraryExtraItemWidget()
{
}

void LibraryExtraItemWidget::setId(const QString& id)
{
	id_ = id;
}

QString LibraryExtraItemWidget::getId() const
{
	return id_;
}

void LibraryExtraItemWidget::setTitle(const QString& title)
{
	ui_.nameLabel->setText(title);
}

QString LibraryExtraItemWidget::getTitle() const
{
	return ui_.nameLabel->text();
}

void LibraryExtraItemWidget::mousePressEvent(QMouseEvent*)
{
}

void LibraryExtraItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
}

void LibraryExtraItemWidget::resizeEvent(QResizeEvent*)
{
}

void LibraryExtraItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryExtraItemWidget::onSetup()
{
	int state = ExtraState::downloaded;
	if (!context_->changeExtraState(id_.toStdString(), state, ExtraState::configuring)) {
		context_->promptExtraState(id_.toStdString(), state);
		return;
	}

	context_->setupExtra(id_.toStdString());

	state = ExtraState::configuring;
	context_->changeExtraState(id_.toStdString(), state, ExtraState::downloaded);
}

void LibraryExtraItemWidget::onBrowse()
{
	std::string path = context_->extraPath(id_.toStdString());
	QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromLocal8Bit(path.c_str())));
}

void LibraryExtraItemWidget::onRemove()
{
	QMessageBox mb(QMessageBox::Question, "Base",
		tr("Are you sure you want to remove this Extra ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes|QMessageBox::No);

	mb.setDefaultButton(QMessageBox::No);

	int ret = mb.exec();
	if (ret != QMessageBox::Yes) {
		return;
	}

	context_->removeExtraFromGui(id_.toStdString());

	int state = ExtraState::downloaded;
	if (context_->changeExtraState(id_.toStdString(), state, ExtraState::not_downloaded))
	{
		fs::path p = context_->extraPath(id_.toStdString());
		if (fs::exists(p)) {
			fs::remove_all(p);
		}
	}	
}

