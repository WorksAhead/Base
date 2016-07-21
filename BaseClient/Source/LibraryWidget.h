#ifndef LIBRARYWIDGET_HEADER_
#define LIBRARYWIDGET_HEADER_

#include "Context.h"

#include <QWidget>

// forward declaration

class LibraryWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit LibraryWidget(ContextPtr, QWidget* parent = 0);
	~LibraryWidget();

private:
	

	ContextPtr context_;
};

#endif // LIBRARYWIDGET_HEADER_

