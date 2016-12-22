#ifndef VIDEOPLAYERWIDGET_HEADER_
#define VIDEOPLAYERWIDGET_HEADER_

#include "ui_VideoPlayerWidgetControl.h"

#include <QWidget>
#include <QSlider>
#include <QTimer>
#include <QProgressBar>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>

// forward declaration
class ProgressBar;

class VideoPlayerWidget : public QWidget {
private:
	Q_OBJECT

public:
	explicit VideoPlayerWidget(QWidget* parent = 0);
	~VideoPlayerWidget();

public Q_SLOTS:
	void open(const QString& url);
	void openAndPlay(const QString& url);
	void play();
	void pause();
	void stop();

protected:
	virtual bool event(QEvent*);

	virtual void mousePressEvent(QMouseEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
	void onSeek(int);
	void onPlayPause();
	void onStop();
	void onMute(bool);
	void onVolume(int);
	void onLengthChanged(int);
	void onTimeChanged(int);
	void onTimeout();

private:
	VlcInstance* instance_;
	VlcMediaPlayer* player_;
	VlcWidgetVideo* video_;

	QWidget* controlWidget_;
	Ui::VideoPlayerWidgetControl control_;

	QTimer* timer_;

	int controlVisible_;
};

#endif // VIDEOPLAYERWIDGET_HEADER_

