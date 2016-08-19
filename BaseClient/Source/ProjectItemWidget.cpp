#include "ProjectItemWidget.h"
#include "CreateProjectDialog.h"
#include "QtUtils.h"

#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

namespace fs = boost::filesystem;

ProjectItemWidget::ProjectItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	ui_.nameLabel->setToolTip(tr("Double click to edit"));

	QMenu* menu = new QMenu;
	ui_.openButton->setMenu(menu);

	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.openButton, &QPushButton::clicked, this, &ProjectItemWidget::onOpen);
	QObject::connect(removeAction, &QAction::triggered, this, &ProjectItemWidget::onRemove);

	updateTips();
}

ProjectItemWidget::~ProjectItemWidget()
{
}

void ProjectItemWidget::setContentId(const QString& id)
{
	contentId_ = id;
}

const QString& ProjectItemWidget::contentId() const
{
	return contentId_;
}

void ProjectItemWidget::setProjectId(const QString& id)
{
	projectId_ = id;
}

const QString& ProjectItemWidget::projectId() const
{
	return projectId_;
}

void ProjectItemWidget::setName(const QString& text)
{
	ui_.nameLabel->setText(text);
	updateTips();
}

QString ProjectItemWidget::name() const
{
	return ui_.nameLabel->text();
}

void ProjectItemWidget::setImage(const QPixmap& pixmap)
{
	ui_.thumbnailViewer->setPixmap(pixmap);
}

void ProjectItemWidget::mousePressEvent(QMouseEvent*)
{
}

void ProjectItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QPoint pos = ui_.nameLabel->mapFrom(this, e->pos());
		if (ui_.nameLabel->rect().contains(pos))
		{
			ui_.nameLabel->setVisible(false);
			QLineEdit* edit = new QLineEdit;
			edit->setFont(ui_.nameLabel->font());
			edit->setText(ui_.nameLabel->text());
			ui_.verticalLayout->insertWidget(0, edit);
			QObject::connect(edit, &QLineEdit::editingFinished, [this, edit](){
				context_->renameProject(projectId_.toStdString(), edit->text().toLocal8Bit().data());
				ui_.nameLabel->setText(edit->text());
				ui_.nameLabel->setVisible(true);
				edit->deleteLater();
				updateTips();
			});
			edit->setFocus();
		}
	}
}

void ProjectItemWidget::resizeEvent(QResizeEvent*)
{
}

void ProjectItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ProjectItemWidget::onOpen()
{
	ProjectInfo pi;
	if (!context_->getProject(pi, projectId_.toStdString())) {
		return;
	}

	std::string engineName;
	std::string engineVersion;
	{
		std::istringstream stream(pi.defaultEngineVersion);
		std::getline(stream, engineName);
		std::getline(stream, engineVersion);
		if (engineName.empty() || engineVersion.empty()) {
			QMessageBox::information(this, "Base", tr("No default Engine."));
		}
	}

	int state = context_->getEngineState(EngineVersion(engineName, engineVersion));

	if (state == EngineState::installed)
	{
		std::string startup = pi.startup;
		boost::ireplace_all(startup, "$(EngineDir)", context_->enginePath(EngineVersion(engineName, engineVersion)));
		boost::ireplace_all(startup, "$(ProjectDir)", pi.location);
		std::string command;
		std::string workDir;
		std::istringstream stream(startup);
		getline(stream, command);
		getline(stream, workDir);

		QStringList args = parseCombinedArgString(QString::fromLocal8Bit(command.c_str()));
		QString program = args.first();
		args.removeFirst();

		QProcess::startDetached(program, args, QString::fromLocal8Bit(workDir.c_str()));
	}
	else if (state == EngineState::not_installed)
	{
		const int ret = QMessageBox::question(
			0, "Base",
			QString(tr("The default Engine (%1 %2) to open this project is not installed, "
			"do you want to install it now ?")).arg(engineName.c_str()).arg(engineVersion.c_str()),
			QMessageBox::Yes|QMessageBox::Default, QMessageBox::No);

		if (ret == QMessageBox::Yes) {
			context_->installEngine(EngineVersion(engineName, engineVersion));
		}

		return;
	}
	else if (state == EngineState::removing)
	{
		QMessageBox::information(this, "Base",
			QString(tr("The default Engine (%1 %2) to open this project is now removing, "
			"you can reinstall after removing the Engine.")).arg(engineName.c_str()).arg(engineVersion.c_str()));

		return;
	}
	else if (state == EngineState::installing)
	{
		QMessageBox::information(this, "Base",
			QString(tr("The default Engine (%1 %2) to open this project is now installing, "
			"please wait.")).arg(engineName.c_str()).arg(engineVersion.c_str()));

		return;
	}
}

void ProjectItemWidget::onRemove()
{
	QMessageBox mb(QMessageBox::Question, "Base",
		tr("Are you sure you want to remove this project ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes|QMessageBox::No);

	mb.setDefaultButton(QMessageBox::No);
	//mb.setCheckBox(new QCheckBox("Remove project directory from disk"));

	int ret = mb.exec();
	if (ret != QMessageBox::Yes) {
		return;
	}

	context_->removeProject(projectId_.toStdString(), false/*mb.checkBox()->isChecked()*/);
}

void ProjectItemWidget::updateTips()
{
	QString tips;

	Rpc::ContentInfo ci;
	Rpc::ErrorCode ec = context_->session->getContentInfo(contentId_.toStdString(), ci);
	if (ec == Rpc::ec_success)
	{
		if (!tips.isEmpty()) {
			tips += "\n";
		}
		tips +=
			QString(tr("Supported Engine Versions:")) +
			"\n" +
			ci.engineName.c_str() +
			" " +
			ci.engineVersion.c_str();
	}

	ProjectInfo pi;
	if (context_->getProject(pi, projectId_.toStdString()))
	{
		if (!tips.isEmpty()) {
			tips += "\n";
		}
		tips +=
			QString(tr("Location:")) +
			"\n" +
			QString::fromLocal8Bit(pi.location.c_str());
	}

	if (!tips.isEmpty()) {
		ui_.thumbnailViewer->setToolTip(tips);
	}
}

