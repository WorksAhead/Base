#ifndef PAGECONTENTITEMWIDGET_HEADER_
#define PAGECONTENTITEMWIDGET_HEADER_

#include <QWidget>
#include <QImage>
#include <QBoxLayout>
#include <QString>
#include <QMovie>

class PageContentItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit PageContentItemWidget(QWidget* parent = 0);
	~PageContentItemWidget();

	void setId(const QString&);
	QString id() const;

	void setText(const QString&);
	QString text() const;

	void setBackground(QPixmap*);
	void setBackground(QMovie*);
	bool hasBackground();

	void setSize(int);

private Q_SLOTS:
	void onBackgroundUpdated(QRect);

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void resizeEvent(QResizeEvent*);

private:
	void clear();

private:
	QString id_;
	QString text_;

	QPixmap* bg_;
	QMovie* movie_;

	QPixmap scaledBg_;
	
	int size_;
};

#endif // PAGECONTENTITEMWIDGET_HEADER_

