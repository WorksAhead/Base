#ifndef LIBRARYWIDGET_HEADER_
#define LIBRARYWIDGET_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration
class LibraryContentWidget;

class LibraryWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit LibraryWidget(ContextPtr, QWidget* parent = 0);
	~LibraryWidget();

public Q_SLOTS:
	void addContent(const QString& id);
	void removeContent(const QString& id);
	void addProject(const QString& id);
	void removeProject(const QString& id);

private:
	ContextPtr context_;

	LibraryContentWidget* libraryContent_;
};

#endif // LIBRARYWIDGET_HEADER_

