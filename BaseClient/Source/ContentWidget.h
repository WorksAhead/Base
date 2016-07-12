#ifndef CONTENTWIDGET_HEADER_
#define CONTENTWIDGET_HEADER_

#include "ui_ContentWidget.h"

#include <QString>
#include <QVector>
#include <QPixmap>
#include <QBoxLayout>

class ContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit ContentWidget(QWidget* parent = 0);
	~ContentWidget();

	void setId(const QString& id);
	const QString& id() const;

	void setTitle(const QString& text);
	void setSummary(const QString& text);
	void setDescription(const QString& text);

	void setImage(int index, const QPixmap&);
	void setImageCount(int count);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);

private:
	Ui::ContentWidget ui_;
	QBoxLayout* thumbnailLayout_;
	QWidget* thumbnailWidget_;
	QString id_;
	QVector<QPixmap> screenshots_;
};

#endif // CONTENTWIDGET_HEADER_

