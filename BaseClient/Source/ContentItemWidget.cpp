#include "ContentItemWidget.h"
#include "CreateProjectDialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

ContentItemWidget::ContentItemWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	QMenu* menu = new QMenu;
	ui_.createButton->setMenu(menu);

	QAction* removeAction = menu->addAction("Remove");

	QObject::connect(ui_.createButton, &QPushButton::clicked, this, &ContentItemWidget::onCreate);
	QObject::connect(removeAction, &QAction::triggered, this, &ContentItemWidget::onRemove);
}

ContentItemWidget::~ContentItemWidget()
{
}

void ContentItemWidget::setContentId(const QString& id)
{
	contentId_ = id;
}

const QString& ContentItemWidget::contentId() const
{
	return contentId_;
}

void ContentItemWidget::setTitle(const QString& text)
{
	ui_.titleLabel->setText(text);
}

QString ContentItemWidget::title() const
{
	return ui_.titleLabel->text();
}

void ContentItemWidget::setImage(const QPixmap& pixmap)
{
	ui_.thumbnailViewer->setPixmap(pixmap);
}

void ContentItemWidget::mousePressEvent(QMouseEvent*)
{
}

void ContentItemWidget::resizeEvent(QResizeEvent*)
{
}

void ContentItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContentItemWidget::onCreate()
{
	CreateProjectDialog d;

	std::string t(title().toLocal8Bit().data());

	t.erase(std::remove_if(t.begin(), t.end(), boost::is_any_of(R"(\/:*?"<>|)")), t.end());

	d.setDirectory(QString::fromLocal8Bit(t.c_str()));

	int ret = d.exec();
	if (ret == 0) {
		return;
	}

	context_->createProject(contentId_.toStdString(), title().toLocal8Bit().data(), d.location().toLocal8Bit().data());
}

void ContentItemWidget::onRemove()
{
	const int rc = QMessageBox::question(
		0, "Base",
		tr("Are you sure you want to remove this content ?\nWarning: This operation cannot be undone."),
		QMessageBox::Yes, QMessageBox::No|QMessageBox::Default);

	if (rc != QMessageBox::Yes) {
		return;
	}

	int state = ContentState::downloaded;
	if (context_->changeContentState(contentId_.toStdString(), state, ContentState::not_downloaded))
	{
		fs::path p = context_->contentPath(contentId_.toStdString());
		if (fs::exists(p)) {
			fs::remove_all(p);
		}
	}
}

