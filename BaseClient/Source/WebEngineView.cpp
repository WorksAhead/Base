#include "WebEngineView.h"

WebEngineView::WebEngineView(QWidget* parent) : QWebEngineView(parent)
{
}

WebEngineView::~WebEngineView()
{
}

QWebEngineView* WebEngineView::createWindow(QWebEnginePage::WebWindowType type)
{
	if (type == QWebEnginePage::WebBrowserWindow || type == QWebEnginePage::WebBrowserTab || type == QWebEnginePage::WebBrowserBackgroundTab)
	{
		WebEngineView* webView = new WebEngineView(parentWidget());
		webView->setObjectName(QStringLiteral("webView"));
		Q_EMIT newViewCreated(webView);
		return webView;
	}

	return 0;
}
