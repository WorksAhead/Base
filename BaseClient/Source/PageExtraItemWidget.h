#ifndef PAGEEXTRAITEMWIDGET_HEADER_
#define PAGEEXTRAITEMWIDGET_HEADER_

#include <QWidget>
#include <QImage>
#include <QBoxLayout>
#include <QString>

class PageExtraItemWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageExtraItemWidget(QWidget* parent = 0);
	~PageExtraItemWidget();

	void setId(const QString& id);
	QString id() const;

	void setText(const QString& title);
	QString text() const;

	void setBackground(const QPixmap&);

	void setSize(int);

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
	int size_;
};

#endif // PAGEEXTRAITEMWIDGET_HEADER_

