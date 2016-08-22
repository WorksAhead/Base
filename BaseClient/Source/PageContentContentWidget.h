#ifndef PAGECONTENTCONTENTWIDGET_HEADER_
#define PAGECONTENTCONTENTWIDGET_HEADER_

#include "Context.h"

#include "ui_PageContentContentWidget.h"

#include <QList>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QBoxLayout>

class PageContentContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageContentContentWidget(ContextPtr context, QWidget* parent = 0);
	~PageContentContentWidget();

	void setContentId(const QString& id);
	const QString& contentId() const;

	void setTitle(const QString& text);
	void setSummary(const QString& text);
	void setDescription(const QString& text);

	void setEngineVersion(int index, const QString& name, const QString& version);
	void setEngineVersionCount(int count);

	void setImage(int index, const QPixmap&);
	void setImageCount(int count);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onDownload();
	void onCopyId();
	void onCopySummary();

private:
	ContextPtr context_;
	Ui::PageContentContentWidget ui_;
	QBoxLayout* thumbnailLayout_;
	QWidget* thumbnailWidget_;
	QString contentId_;
	QPair<QString, QString> firstEngineVersion_;
	QVector<QPixmap> screenshots_;
};

#endif // PAGECONTENTCONTENTWIDGET_HEADER_

