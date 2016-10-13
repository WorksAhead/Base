#include "PageContentWidget.h"
#include "PageContentBrowserWidget.h"
#include "PageContentContentWidget.h"
#include "URLUtils.h"
#include "QtUtils.h"

#include <QPainter>
#include <QMouseEvent>
#include <QClipboard>
#include <QMenu>

#include <boost/algorithm/string.hpp>

#include <string>

#define MAX_HISTORY 64

PageContentWidget::PageContentWidget(ContextPtr context, const QString& name, QWidget* parent) : QWidget(parent), context_(context), name_(name)
{
	ui_.setupUi(this);

	QMenu* copyUrlMenu = new QMenu(this);
	QAction* copyBaseUrlAction = copyUrlMenu->addAction("Copy URL");
	QAction* copyHttpUrlAction = copyUrlMenu->addAction("Copy HTTP Redirection URL");

	QAction* copyUrlAction = ui_.urlEdit->addAction(QIcon(":/Icons/CopyLink.png"), QLineEdit::TrailingPosition);

	QObject::connect(copyUrlAction, &QAction::triggered, [this, copyUrlMenu](){
		copyUrlMenu->popup(QCursor::pos());
	});

	QObject::connect(ui_.backButton, &QPushButton::clicked, this, &PageContentWidget::onBack);
	QObject::connect(ui_.forwardButton, &QPushButton::clicked, this, &PageContentWidget::onForward);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageContentWidget::onRefresh);
	QObject::connect(ui_.urlEdit, &QLineEdit::returnPressed, this, &PageContentWidget::onUrlEdited);

	QObject::connect(copyBaseUrlAction, &QAction::triggered, this, &PageContentWidget::onCopyUrl);
	QObject::connect(copyHttpUrlAction, &QAction::triggered, this, &PageContentWidget::onCopyHttpUrl);

	openBrowser();
}

PageContentWidget::~PageContentWidget()
{
}

bool PageContentWidget::openUrl(const QString& url)
{
	std::string path;
	KVMap args;
	if (parseUrl(url.toStdString(), path, args) && path == "base://content/") {
		std::string page;
		if (args.lookupValue(page, "page") && page == name_.toStdString()) {
			std::string id;
			if (args.lookupValue(id, "id")) {
				return openContent(QString::fromStdString(id));
			}
			else {
				std::string category;
				args.lookupValue(category, "category");
				openBrowser(category.c_str());
				return true;
			}
		}
	}

	return false;
}

bool PageContentWidget::openContent(const QString& id)
{
	clearOldAndForwardHistory();

	const QString& cachedUrl = ui_.stackedWidget->currentWidget()->property("cached_url").toString();

	const QString& url = URLQuery("base://content/").arg("page", name_.toStdString()).arg("id", id.toStdString()).str().c_str();

	if (url != cachedUrl)
	{
		PageContentContentWidget* w = new PageContentContentWidget(context_, id);
		w->setProperty("cached_url", url);

		ui_.stackedWidget->addWidget(w);
		ui_.stackedWidget->setCurrentWidget(w);
		ui_.urlEdit->setText(url);
	}
	else
	{
		onRefresh();
	}

	return true;
}

QString PageContentWidget::name()
{
	return name_;
}

void PageContentWidget::mousePressEvent(QMouseEvent* e)
{
}

void PageContentWidget::showEvent(QShowEvent* e)
{
}

void PageContentWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageContentWidget::onCategoryClicked(const QString& category)
{
	openBrowser(category);
}

void PageContentWidget::onContentClicked(const QString& id)
{
	openContent(id);
}

void PageContentWidget::onBack()
{
	int index = ui_.stackedWidget->currentIndex();
	if (index > 0) {
		QWidget* w = ui_.stackedWidget->widget(index - 1);
		ui_.stackedWidget->setCurrentWidget(w);
		ui_.urlEdit->setText(w->property("cached_url").toString());
	}
}

void PageContentWidget::onForward()
{
	int index = ui_.stackedWidget->currentIndex();
	if (index + 1 < ui_.stackedWidget->count()) {
		QWidget* w = ui_.stackedWidget->widget(index + 1);
		ui_.stackedWidget->setCurrentWidget(w);
		ui_.urlEdit->setText(w->property("cached_url").toString());
	}
}

void PageContentWidget::onRefresh()
{
	QWidget* w = ui_.stackedWidget->currentWidget();

	if (qobject_cast<PageContentBrowserWidget*>(w)) {
		((PageContentBrowserWidget*)w)->refresh();
	}
	else if (qobject_cast<PageContentContentWidget*>(w)) {
		((PageContentContentWidget*)w)->refresh();
	}
}

void PageContentWidget::onUrlEdited()
{
	ui_.urlEdit->clearFocus();

	const QString& cachedUrl = ui_.stackedWidget->currentWidget()->property("cached_url").toString();

	if (ui_.urlEdit->text() != cachedUrl) {
		if (!openUrl(ui_.urlEdit->text())) {
			const QString& url = ui_.urlEdit->text();
			ui_.urlEdit->setText(cachedUrl);
			Q_EMIT unresolvedUrl(url);
		}
	}
}

void PageContentWidget::onCopyUrl()
{
	ui_.urlEdit->clearFocus();

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(ui_.urlEdit->text());
}

void PageContentWidget::onCopyHttpUrl()
{
	ui_.urlEdit->clearFocus();

	QString s = qApp->property("BaseClient.HttpUrlRedir").toString();

	std::string url = ui_.urlEdit->text().toStdString();
	percentEncode(url);

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(QString::fromStdString(s.toStdString() + "?q=" + url));
}

void PageContentWidget::openBrowser(const QString& category)
{
	clearOldAndForwardHistory();

	PageContentBrowserWidget* w = new PageContentBrowserWidget(context_, name_, category);

	QString url = URLQuery("base://content/").arg("page", name_.toStdString()).arg("category", category.toStdString()).str().c_str();
	w->setProperty("cached_url", url);

	ui_.stackedWidget->addWidget(w);
	ui_.stackedWidget->setCurrentWidget(w);
	ui_.urlEdit->setText(url);

	QObject::connect(w, &PageContentBrowserWidget::categoryClicked, this, &PageContentWidget::onCategoryClicked);
	QObject::connect(w, &PageContentBrowserWidget::contentClicked, this, &PageContentWidget::onContentClicked);
}

void PageContentWidget::clearOldAndForwardHistory()
{
	while (ui_.stackedWidget->currentIndex() + 1 < ui_.stackedWidget->count()) {
		const int index = ui_.stackedWidget->currentIndex() + 1;
		QWidget* w = ui_.stackedWidget->widget(index);
		ui_.stackedWidget->removeWidget(w);
	}

	while (ui_.stackedWidget->count() > MAX_HISTORY) {
		QWidget* w = ui_.stackedWidget->widget(0);
		ui_.stackedWidget->removeWidget(w);
	}
}

