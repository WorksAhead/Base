#ifndef LIBRARYWIDGET_HEADER_
#define LIBRARYWIDGET_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration
class VTabWidget;
class LibraryContentWidget;
class LibraryEngineWidget;
class LibraryExtraWidget;

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
	void addEngine(const QString& engineName, const QString& engineVersion);
	void removeEngine(const QString& engineName, const QString& engineVersion);
	void addExtra(const QString& id);
	void removeExtra(const QString& id);

protected:
	virtual void paintEvent(QPaintEvent*);

private:
	ContextPtr context_;

	VTabWidget* tabWidget_;

	LibraryContentWidget* libraryContent_;
	LibraryEngineWidget* libraryEngine_;
	LibraryExtraWidget* libraryExtra_;
};

#endif // LIBRARYWIDGET_HEADER_

