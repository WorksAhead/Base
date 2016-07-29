#ifndef LIBRARYCONTENTWIDGET_HEADER_
#define LIBRARYCONTENTWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include "ui_LibraryContentWidget.h"

#include <QMap>
#include <QList>

// forward declaration
class ProjectItemWidget;
class ContentItemWidget;

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
	QMap<QString, ProjectItemWidget*> projectItemWidgets_;
	QMap<QString, QList<ProjectItemWidget*>> projectItemsOfContentItem_;
};

#endif // LIBRARYCONTENTWIDGET_HEADER_

