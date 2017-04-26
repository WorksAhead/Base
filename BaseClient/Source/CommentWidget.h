#ifndef COMMENTWIDGET_HEADER_
#define COMMENTWIDGET_HEADER_

#include "Context.h"

#include <RpcSession.h>

#include "ui_CommentWidget.h"

#include <QStringList>
#include <QTimer>

class CommentWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit CommentWidget(QWidget* parent = 0);
	~CommentWidget();

	void setContext(ContextPtr);

	void setTargetId(const QString&);

public:
	virtual void resizeEvent(QResizeEvent*);

private Q_SLOTS:
	void onTimeout();
	void onSubmit();
	void onAnchorClicked(const QUrl&);

private:
	void refresh();

	void getParagraphs(void*, QStringList&);

private:
	Ui::CommentWidget ui_;

	QTimer* timer_;

	ContextPtr context_;

	Rpc::CommentBrowserPrx browser_;

	QString targetId_;
};

#endif // COMMENTWIDGET_HEADER_

