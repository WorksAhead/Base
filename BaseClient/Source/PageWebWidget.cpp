#include "PageWebWidget.h"

#include <QStyleOption>
#include <QPainter>

#include <QDebug>

PageWebWidget::PageWebWidget(ContextPtr context, const QString& name, QWidget* parent)
	: context_(context), name_(name), QWidget(parent)
{
	ui_.setupUi(this);

	QObject::connect(ui_.backButton, &QPushButton::clicked, this, &PageWebWidget::onBack);
	QObject::connect(ui_.forwardButton, &QPushButton::clicked, this, &PageWebWidget::onForward);
	QObject::connect(ui_.refreshButton, &QPushButton::clicked, this, &PageWebWidget::onRefresh);
	QObject::connect(ui_.homeButton, &QPushButton::clicked, this, &PageWebWidget::onHome);
	QObject::connect(ui_.urlEdit, &QLineEdit::returnPressed, this, &PageWebWidget::onUrlEntered);

	connectView(ui_.webView);
}

PageWebWidget::~PageWebWidget()
{
}

void PageWebWidget::setUrl(const QUrl& url)
{
	url_ = url;
	ui_.webView->load(url);
}

void PageWebWidget::onBack()
{
	ui_.webView->back();
}

void PageWebWidget::onForward()
{
	ui_.webView->forward();
}

void PageWebWidget::onRefresh()
{
	ui_.webView->reload();
}

void PageWebWidget::onHome()
{
	ui_.webView->load(url_);
}

void PageWebWidget::onUrlEntered()
{
	ui_.webView->load(QUrl(ui_.urlEdit->text()));
}

void PageWebWidget::onUrlChanged(const QUrl& url)
{
	ui_.urlEdit->setText(url.toDisplayString());
	ui_.urlEdit->setCursorPosition(0);
}

void PageWebWidget::onLoadStarted()
{

}

void PageWebWidget::onLoadFinished()
{

}

void PageWebWidget::onNewViewCreated(WebEngineView* view)
{
	WebEngineView* oldView = ui_.webView;
	ui_.viewLayout->removeWidget(oldView);
	oldView->deleteLater();

	ui_.webView = view;
	ui_.viewLayout->addWidget(view);
	connectView(view);
}

void PageWebWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PageWebWidget::connectView(WebEngineView* view)
{
	QObject::connect(view, &QWebEngineView::urlChanged, this, &PageWebWidget::onUrlChanged);
	QObject::connect(view, &QWebEngineView::loadStarted, this, &PageWebWidget::onLoadStarted);
	QObject::connect(view, &QWebEngineView::loadFinished, this, &PageWebWidget::onLoadFinished);
	QObject::connect(view, &WebEngineView::newViewCreated, this, &PageWebWidget::onNewViewCreated);
}

