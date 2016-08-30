#ifndef LIBRARYEXTRAWIDGET_HEADER_
#define LIBRARYEXTRAWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include "ui_LibraryExtraWidget.h"

#include <QMap>
#include <QList>

// forward declaration
class LibraryExtraItemWidget;

class LibraryExtraWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryExtraWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryExtraWidget();

public Q_SLOTS:
	void addExtra(const QString& id);
	void removeExtra(const QString& id);

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private:
	void refresh();

private:
	ContextPtr context_;

	Ui::LibraryExtraWidget ui_;
	FlowLayout* extrasLayout_;
	bool firstShow_;

	QMap<QString, LibraryExtraItemWidget*> extraItemWidgets_;
};

#endif // LIBRARYEXTRAWIDGET_HEADER_

