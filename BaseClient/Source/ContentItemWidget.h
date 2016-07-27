#ifndef CONTENTITEMWIDGET_HEADER_
#define CONTENTITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_ContentItemWidget.h"

class ContentItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	ContentItemWidget(ContextPtr context, QWidget* parent = 0);
	~ContentItemWidget();

	void setContentId(const QString& id);
	const QString& contentId() const;

	void setTitle(const QString& text);
	QString title() const;

	void setImage(const QPixmap&);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:

private:
	ContextPtr context_;
	Ui::ContentItemWidget ui_;
	QString contentId_;
};

#endif // CONTENTITEMWIDGET_HEADER_

