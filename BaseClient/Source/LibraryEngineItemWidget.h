#ifndef LIBRARYENGINEITEMWIDGET_HEADER_
#define LIBRARYENGINEITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_LibraryEngineItemWidget.h"

class LibraryEngineItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryEngineItemWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryEngineItemWidget();

	void setEngineVersion(const QPair<QString, QString>& v);
	QPair<QString, QString> getEngineVersion();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onSetup();
	void onUnSetup();
	void onBrowse();
	void onRemove();

private:
	ContextPtr context_;
	Ui::LibraryEngineItemWidget ui_;

	QPair<QString, QString> engineVersion_;
};


#endif // LIBRARYENGINEITEMWIDGET_HEADER_

