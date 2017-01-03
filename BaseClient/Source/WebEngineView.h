#ifndef WEBENGINEVIEW_HEADER_
#define WEBENGINEVIEW_HEADER_

#include <QWebEngineView>

class WebEngineView : public QWebEngineView {
private:
	Q_OBJECT

public:
	WebEngineView(QWidget* parent = 0);
	~WebEngineView();

Q_SIGNALS:
	void newViewCreated(WebEngineView*);

protected:
	virtual QWebEngineView* createWindow(QWebEnginePage::WebWindowType type);
};

#endif // WEBENGINEVIEW_HEADER_

