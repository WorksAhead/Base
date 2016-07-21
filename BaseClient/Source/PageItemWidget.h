#ifndef PAGEITEMWIDGET_HEADER_
#define PAGEITEMWIDGET_HEADER_

#include <QWidget>
#include <QImage>
#include <QBoxLayout>
#include <QString>

class PageItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit PageItemWidget(QWidget* parent = 0);
	~PageItemWidget();

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

#endif // PAGEITEMWIDGET_HEADER_

