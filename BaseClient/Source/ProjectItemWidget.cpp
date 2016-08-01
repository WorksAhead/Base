#include "ProjectItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ProjectItemWidget::ProjectItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

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
}

void ProjectItemWidget::onRemove()
{
	QMessageBox mb(QMessageBox::Question, "Base",
		tr("Are you sure you want to remove this project ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes|QMessageBox::No);

	mb.setDefaultButton(QMessageBox::No);
	mb.setCheckBox(new QCheckBox("Remove project directory from disk"));

	int ret = mb.exec();
	if (ret != QMessageBox::Yes) {
		return;
	}

	context_->removeProject(projectId_.toStdString(), mb.checkBox()->isChecked());
}

void ProjectItemWidget::updateTips()
{
	ProjectInfo pi;
	if (context_->getProject(pi, projectId_.toStdString()))
	{
		ui_.thumbnailViewer->setToolTip(
			name() +
			"\n" +
			QString::fromLocal8Bit(pi.location.c_str()));
	}

	ui_.nameLabel->setToolTip(tr("Double click to edit"));
}

