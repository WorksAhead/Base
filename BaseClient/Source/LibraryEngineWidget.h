#ifndef LIBRARYENGINEWIDGET_HEADER_
#define LIBRARYENGINEWIDGET_HEADER_

#include "Context.h"
#include "FlowLayout.h"

#include "ui_LibraryEngineWidget.h"

#include <QMap>
#include <QList>

// forward declaration
class EngineItemWidget;

class LibraryEngineWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryEngineWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryEngineWidget();

public Q_SLOTS:
	void addEngine(const QString& engineName, const QString& engineVersion);
	void removeEngine(const QString& engineName, const QString& engineVersion);

protected:
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private:
	void refresh();

private:
	ContextPtr context_;

	Ui::LibraryEngineWidget ui_;
	FlowLayout* enginesLayout_;
	bool firstShow_;

	QMap<QString, EngineItemWidget*> engineItemWidgets_;
};

#endif // LIBRARYENGINEWIDGET_HEADER_

