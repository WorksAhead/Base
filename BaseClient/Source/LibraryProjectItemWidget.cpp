#include "LibraryProjectItemWidget.h"
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
#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <sstream>

namespace fs = boost::filesystem;

LibraryProjectItemWidget::LibraryProjectItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	ui_.nameLabel->setToolTip(tr("Double click to edit"));

	QMenu* menu = new QMenu;
	ui_.openButton->setMenu(menu);

	QAction* switchEngineVerAction = menu->addAction("Switch Engine version");
	QAction* browserAction = menu->addAction("Browse");
	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.openButton, &QPushButton::clicked, this, &LibraryProjectItemWidget::onOpen);
	QObject::connect(switchEngineVerAction, &QAction::triggered, this, &LibraryProjectItemWidget::onSwitchEngineVersion);
	QObject::connect(browserAction, &QAction::triggered, this, &LibraryProjectItemWidget::onBrowse);
	QObject::connect(removeAction, &QAction::triggered, this, &LibraryProjectItemWidget::onRemove);

	updateTips();
}

LibraryProjectItemWidget::~LibraryProjectItemWidget()
{
}

void LibraryProjectItemWidget::setContentId(const QString& id)
{
	contentId_ = id;
}

const QString& LibraryProjectItemWidget::contentId() const
{
	return contentId_;
}

void LibraryProjectItemWidget::setProjectId(const QString& id)
{
	projectId_ = id;
}

const QString& LibraryProjectItemWidget::projectId() const
{
	return projectId_;
}

void LibraryProjectItemWidget::setName(const QString& text)
{
	ui_.nameLabel->setText(text);
	updateTips();
}

QString LibraryProjectItemWidget::name() const
{
	return ui_.nameLabel->text();
}

void LibraryProjectItemWidget::setImage(const QPixmap& pixmap)
{
	ui_.thumbnailViewer->setPixmap(pixmap);
}

void LibraryProjectItemWidget::mousePressEvent(QMouseEvent*)
{
}

void LibraryProjectItemWidget::mouseDoubleClickEvent(QMouseEvent* e)
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

void LibraryProjectItemWidget::resizeEvent(QResizeEvent*)
{
}

void LibraryProjectItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryProjectItemWidget::onOpen()
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
			return;
		}
	}

	if (engineVersion == "(Custom)")
	{
		std::string startup = pi.startup;

		if (boost::icontains(startup, "$(EngineDir)"))
		{
			QString s = QFileDialog::getExistingDirectory(this, QString("Select custom %1").arg(engineName.c_str()));

			if (s.isEmpty()) {
				return;
			}

			boost::ireplace_all(startup, "$(EngineDir)", s.toStdString());

			context_->changeProjectStartup(projectId_.toStdString(), startup);
		}

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

		return;
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

void LibraryProjectItemWidget::onSwitchEngineVersion()
{
	ProjectInfo pi;
	if (!context_->getProject(pi, projectId_.toStdString())) {
		return;
	}

	std::string defaultEngineName;
	std::string defaultEngineVersion;
	{
		std::istringstream stream(pi.defaultEngineVersion);
		std::getline(stream, defaultEngineName);
		std::getline(stream, defaultEngineVersion);
	}

	Rpc::ContentInfo ci;
	if (context_->session->getContentInfo(contentId_.toStdString(), ci) != Rpc::ec_success) {
		return;
	}

	std::vector<std::string> versions;
	boost::split(versions, ci.engineVersion, boost::is_any_of("|"));

	versions.push_back("(Custom)");

	QStringList list;

	int current = 0;

	for (int i = 0; i < versions.size(); ++i)
	{
		list.append(versions[i].c_str());

		if (defaultEngineVersion == versions[i]) {
			current = i;
		}
	}

	bool ok;

	QString item = QInputDialog::getItem(this, tr("Switch Engine version"), ci.engineName.c_str(), list, current, false, &ok, Qt::WindowTitleHint);

	if (ok && !item.isEmpty()) {
		context_->changeProjectDefaultEngineVersion(projectId_.toStdString(), EngineVersion(ci.engineName, item.toStdString()));
		context_->changeProjectStartup(projectId_.toStdString(), ci.startup);
	}
}

void LibraryProjectItemWidget::onBrowse()
{
	ProjectInfo pi;
	if (!context_->getProject(pi, projectId_.toStdString())) {
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromLocal8Bit(pi.location.c_str())));
}

void LibraryProjectItemWidget::onRemove()
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

	context_->removeProjectFromGui(projectId_.toStdString());
	context_->removeProject(projectId_.toStdString(), false/*mb.checkBox()->isChecked()*/);
}

void LibraryProjectItemWidget::updateTips()
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

