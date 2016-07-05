#ifndef PAGEITEM_HEADER_
#define PAGEITEM_HEADER_

#include <QWidget>
#include <QImage>
#include <QBoxLayout>
#include <QString>

class PageItem : public QWidget {
private:
	Q_OBJECT

public:
	explicit PageItem(QWidget* parent = 0);
	~PageItem();

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

#endif // PAGEITEM_HEADER_

