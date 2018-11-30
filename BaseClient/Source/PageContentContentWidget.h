#ifndef PAGECONTENTCONTENTWIDGET_HEADER_
#define PAGECONTENTCONTENTWIDGET_HEADER_

#include "Context.h"

#include "ui_PageContentContentWidget.h"

#include <QList>
#include <QString>
#include <QVector>
#include <QPixmap>
#include <QBoxLayout>
#include <QScrollArea>

// forward declaration
class VideoPlayerWidget;
class ImageViewerWidget;

class PageContentContentWidget : public QWidget {
private:
	Q_OBJECT

public:
	PageContentContentWidget(ContextPtr context, const QString& contentId, QWidget* parent = 0);
	~PageContentContentWidget();

	void refresh();
	void cancel();
	void restore();

	const QString& contentId() const;

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onImageLoaded(const QString& id, int index, const QPixmap&);
	void onVideoSnapshot(const QString& filename);
	void onDownload();
	void onAnchorClicked(const QUrl&);
	void onEdit();
	void onChangeDisplayPriority();
	void onSubmitNew();

private:
	void refreshRelatedVersions(int);
	void initView();
	VideoPlayerWidget* findVideoPlayerWidget(int);
	ImageViewerWidget* findImageViewerWidget();
	void presentImage(const QPixmap&);
	void presentVideo(const QString&);
	void takeVideoSnapshot();
	QStringList queryEngineVersions();

private:
	ContextPtr context_;
	QString contentId_;

	Ui::PageContentContentWidget ui_;

	QScrollArea* scrollArea_;
	QBoxLayout* thumbnailLayout_;
	QWidget* thumbnailWidget_;

	QPair<QString, QString> supportedEngineVersion_;

	QVector<QString> videos_;
	QVector<QPixmap> screenshots_;

	int videoSnapshot_;

	bool firstShow_;
};

#endif // PAGECONTENTCONTENTWIDGET_HEADER_

