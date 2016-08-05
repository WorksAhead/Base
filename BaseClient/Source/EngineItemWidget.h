#ifndef ENGINEITEMWIDGET_HEADER_
#define ENGINEITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_EngineItemWidget.h"

class EngineItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	EngineItemWidget(ContextPtr context, QWidget* parent = 0);
	~EngineItemWidget();

	void setEngineVersion(const QPair<QString, QString>& v);
	QPair<QString, QString> getEngineVersion();

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onShow();
	void onRemove();

private:
	ContextPtr context_;
	Ui::EngineItemWidget ui_;

	QPair<QString, QString> engineVersion_;
};

#endif // ENGINEITEMWIDGET_HEADER_

