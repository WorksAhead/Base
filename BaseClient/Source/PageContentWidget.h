#ifndef PAGECONTENTWIDGET_HEADER_
#define PAGECONTENTWIDGET_HEADER_

#include "ui_PageContentWidget.h"

#include <RpcSession.h>

#include "Context.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <QScrollArea>

#include <limits.h>

// forward declaration
class ASyncDownloadTask;
class FlowLayout;
class PageContentItemWidget;
class PageContentContentWidget;

class PageContentWidget : public QWidget {
private:
	Q_OBJECT

private:
	typedef QPair<QString, int> ImageIndex;

public:
	PageContentWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageContentWidget();

	bool openUrl(const QString& url);
	bool openContent(const QString& id);

	QString name();

	CategoryFilterWidget* categoryFilterWidget();

Q_SIGNALS:
	void unresolvedUrl(const QString&);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onCategoryChanged();
	void onContentClicked(const QString& id);
	void onBack();
	void onForward();
	void onRefresh();
	void onUrlEdited();
	void onCopyUrl();
	void onCopyHttpUrl();

private:
	void openBrowser(const QString& category = "");
	void clearOldAndForwardHistory();
	void restore(int);

private:
	ContextPtr context_;
	QString name_;

	Ui::PageContentWidget ui_;
};

#endif // PAGECONTENTWIDGET_HEADER_

