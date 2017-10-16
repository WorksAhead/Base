#ifndef PAGEEXTRAWIDGET_HEADER_
#define PAGEEXTRAWIDGET_HEADER_

#include "ui_PageExtraWidget.h"

#include <RpcSession.h>

#include "Context.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTimer>

// forward declaration
class PageExtraItemWidget;
class FlowLayout;

class PageExtraWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageExtraWidget(ContextPtr context, const QString& name, QWidget* parent = 0);
	~PageExtraWidget();

	bool openUrl(const QString& url);

	CategoryFilterWidget* categoryFilterWidget();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onCategoryChanged();
	void onScroll(int);
	void onBack();
	void onRefresh();
	void onInstall();
	void onImageLoaded(const QString& id, const QPixmap&);
	void onTimeout();
	void onAnchorClicked(const QUrl&);

private:
	void refreshRelatedVersions(int);
	bool showExtra(const QString& id);
	void showMore(int);
	void clear();
	QString currentCategory();

private:
	ContextPtr context_;
	QString name_;
	FlowLayout* flowLayout_;

	Rpc::ExtraBrowserPrx browser_;

	QMap<QString, PageExtraItemWidget*> items_;

	QTimer* timer_;

	Ui::PageExtraWidget ui_;

	QString currentId_;

	bool firstShow_;
	int count_;
};

#endif // PAGEEXTRAWIDGET_HEADER_

