#ifndef PROJECTITEMWIDGET_HEADER_
#define PROJECTITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_ProjectItemWidget.h"

class ProjectItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	ProjectItemWidget(ContextPtr context, QWidget* parent = 0);
	~ProjectItemWidget();

	void setContentId(const QString& id);
	const QString& contentId() const;

	void setProjectId(const QString& id);
	const QString& projectId() const;

	void setName(const QString& text);
	QString name() const;

	void setImage(const QPixmap&);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onOpen();
	void onRemove();

private:
	ContextPtr context_;
	Ui::ProjectItemWidget ui_;
	QString contentId_;
	QString projectId_;
};

#endif // PROJECTITEMWIDGET_HEADER_

