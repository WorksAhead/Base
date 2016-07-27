#ifndef LIBRARYCONTENTWIDGET_HEADER_
#define LIBRARYCONTENTWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include "ui_LibraryContentWidget.h"

#include <QMap>

// forward declaration
class ContentItemWidget;

class LibraryContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryContentWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryContentWidget();

public Q_SLOTS:
	void addContent(const QString& id);

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onImageLoaded(const QString& id, int index, const QPixmap&);

private:
	void refresh();

private:
	ContextPtr context_;

	Ui::LibraryContentWidget ui_;
	FlowLayout* projectsLayout_;
	FlowLayout* contentsLayout_;
	bool firstShow_;

	QMap<QString, ContentItemWidget*> contentItemWidgets_;
};

#endif // LIBRARYCONTENTWIDGET_HEADER_

