#ifndef CONTENTWIDGET_HEADER_
#define CONTENTWIDGET_HEADER_

#include "Context.h"

#include "ui_ContentWidget.h"

#include <QList>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QBoxLayout>

class ContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	ContentWidget(ContextPtr context, QWidget* parent = 0);
	~ContentWidget();

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
	void onInstallEngine();
	void onCopyId();
	void onCopySummary();

private:
	ContextPtr context_;
	Ui::ContentWidget ui_;
	QBoxLayout* thumbnailLayout_;
	QWidget* thumbnailWidget_;
	QString contentId_;
	QVector<QPair<QString, QString>> engineVersions_;
	QVector<QPixmap> screenshots_;
};

#endif // CONTENTWIDGET_HEADER_

