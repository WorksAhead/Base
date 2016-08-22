#ifndef CONTENTITEMWIDGET_HEADER_
#define CONTENTITEMWIDGET_HEADER_

#include "Context.h"

#include "ui_LibraryContentItemWidget.h"

class LibraryContentItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	LibraryContentItemWidget(ContextPtr context, QWidget* parent = 0);
	~LibraryContentItemWidget();

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
	void onCreate();
	void onRemove();

private:
	void updateTips();

private:
	ContextPtr context_;
	Ui::LibraryContentItemWidget ui_;
	QString contentId_;
};

#endif // CONTENTITEMWIDGET_HEADER_

