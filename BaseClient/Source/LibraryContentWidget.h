#ifndef LIBRARYCONTENTWIDGET_HEADER_
#define LIBRARYCONTENTWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include "ui_LibraryContentWidget.h"

#include <QMap>
#include <QList>
#include <QMovie>

// forward declaration
class LibraryProjectItemWidget;
class LibraryContentItemWidget;

class LibraryContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryContentWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryContentWidget();

public Q_SLOTS:
	void addContent(const QString& id);
	void removeContent(const QString& id);
	void addProject(const QString& id);
	void removeProject(const QString& id);

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onImageLoaded(const QString& id, int index, QPixmap*);
	void onAnimationLoaded(const QString& id, int index, QMovie*);

private:
	void refresh();

private:
	ContextPtr context_;

	Ui::LibraryContentWidget ui_;
	FlowLayout* projectsLayout_;
	FlowLayout* contentsLayout_;
	bool firstShow_;

	QMap<QString, LibraryContentItemWidget*> contentItemWidgets_;
	QMap<QString, LibraryProjectItemWidget*> projectItemWidgets_;
	QMap<QString, QList<LibraryProjectItemWidget*>> projectItemsOfContentItem_;
};

#endif // LIBRARYCONTENTWIDGET_HEADER_

