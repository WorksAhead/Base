#ifndef LIBRARYEXTRAITEMWIDGET_HEADER_
#define LIBRARYEXTRAITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_LibraryExtraItemWidget.h"

class LibraryExtraItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryExtraItemWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryExtraItemWidget();

	void setId(const QString& id);
	QString getId() const;

	void setTitle(const QString& title);
	QString getTitle() const;

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onSetup();
	void onBrowse();
	void onRemove();

private:
	ContextPtr context_;
	Ui::LibraryExtraItemWidget ui_;

	QString id_;
};

#endif // LIBRARYEXTRAITEMWIDGET_HEADER_

