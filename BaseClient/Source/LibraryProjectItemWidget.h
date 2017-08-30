#ifndef LIBRARYPROJECTITEMWIDGET_HEADER_
#define LIBRARYPROJECTITEMWIDGET_HEADER_

#include "Context.h"
#include "Script.h"

#include "ui_LibraryProjectItemWidget.h"

class LibraryProjectItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryProjectItemWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryProjectItemWidget();

	void setContentId(const QString& id);
	const QString& contentId() const;

	void setProjectId(const QString& id);
	const QString& projectId() const;

	void setName(const QString& text);
	QString name() const;

	void setImage(const QPixmap&);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onOpen();
	void onSwitchEngineVersion();
	void onBrowse();
	void onRemove();

private:
	void updateTips();
	bool checkCustomEngineLocation(const QString&);

private:
	ContextPtr context_;
	Ui::LibraryProjectItemWidget ui_;
	QString contentId_;
	QString projectId_;
	Script script_;
};

#endif // LIBRARYPROJECTITEMWIDGET_HEADER_

