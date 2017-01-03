#ifndef PAGEWEBWIDGET_HEADER_
#define PAGEWEBWIDGET_HEADER_

#include "Context.h"

#include "ui_PageWebWidget.h"

class PageWebWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageWebWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageWebWidget();

public:
	void setUrl(const QUrl&);

private Q_SLOTS:
	void onBack();
	void onForward();
	void onRefresh();
	void onHome();
	void onUrlEntered();
	void onUrlChanged(const QUrl&);
	void onLoadStarted();
	void onLoadFinished();
	void onNewViewCreated(WebEngineView*);

protected:
	virtual void paintEvent(QPaintEvent*);

private:
	void connectView(WebEngineView*);

private:
	ContextPtr context_;
	QString name_;
	QUrl url_;
	Ui::PageWebWidget ui_;
};

#endif // PAGEWEBWIDGET_HEADER_

