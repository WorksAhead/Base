#ifndef PAGECONTENTITEMWIDGET_HEADER_
#define PAGECONTENTITEMWIDGET_HEADER_

#include <QWidget>
#include <QImage>
#include <QBoxLayout>
#include <QString>

class PageContentItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit PageContentItemWidget(QWidget* parent = 0);
	~PageContentItemWidget();

	void setId(const QString&);
	QString id() const;

	void setText(const QString&);

	void setBackground(const QPixmap&);

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void resizeEvent(QResizeEvent*);

private:
	void updateBackground(const QSize&);

private:
	QString id_;
	QString text_;
	QPixmap bg_;
	QPixmap scaledBg_;
};

#endif // PAGECONTENTITEMWIDGET_HEADER_

