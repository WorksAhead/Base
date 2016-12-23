#include "VideoPlayerWidget.h"
#include "ProgressBar.h"

#include <VLCQtCore/Video.h>
#include <VLCQtCore/Audio.h>

#include <QBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QTime>

#include <algorithm>

VideoPlayerWidget::VideoPlayerWidget(QWidget* parent) : QWidget(parent)
{
	setAttribute(Qt::WA_Hover);

	instance_ = new VlcInstance(VlcCommon::args(), this);
	player_ = new VlcMediaPlayer(instance_);

	video_ = new VlcWidgetVideo;

	player_->setVideoWidget(video_);
	video_->setMediaPlayer(player_);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->addWidget(video_);
	setLayout(layout);

	controlWidget_ = new QWidget(this);
	controlWidget_->setAutoFillBackground(true);
	control_.setupUi(controlWidget_);

	timer_ = new QTimer(this);

	QObject::connect(control_.seekBar, &ProgressBar::clicked, this, &VideoPlayerWidget::onSeek);
	QObject::connect(control_.playButton, &QPushButton::clicked, this, &VideoPlayerWidget::onPlayPause);
	QObject::connect(control_.stopButton, &QPushButton::clicked, this, &VideoPlayerWidget::onStop);
	QObject::connect(control_.muteButton, &QPushButton::toggled, this, &VideoPlayerWidget::onMute);
	QObject::connect(control_.volumeSlider, &QSlider::valueChanged, this, &VideoPlayerWidget::onVolume);

	QObject::connect(player_, &VlcMediaPlayer::lengthChanged, this, &VideoPlayerWidget::onLengthChanged);
	QObject::connect(player_, &VlcMediaPlayer::timeChanged, this, &VideoPlayerWidget::onTimeChanged);

	QObject::connect(timer_, &QTimer::timeout, this, &VideoPlayerWidget::onTimeout);

	control_.volumeSlider->setValue(50);
	player_->audio()->setVolume(50);
	player_->audio()->setMute(true);

	timer_->start(10);

	controlVisible_ = 0;
	snapshotTimePoint_ = -1;
}

VideoPlayerWidget::~VideoPlayerWidget()
{
}

void VideoPlayerWidget::open(const QString& url)
{
	VlcMedia* media = new VlcMedia(url, instance_);
	player_->openOnly(media);
}

void VideoPlayerWidget::openAndPlay(const QString& url)
{
	VlcMedia* media = new VlcMedia(url, instance_);
	player_->open(media);
}

void VideoPlayerWidget::play()
{
	player_->play();
}

void VideoPlayerWidget::pause()
{
	player_->pause();
}

void VideoPlayerWidget::stop()
{
	player_->stop();
}

void VideoPlayerWidget::takeSnapshot(const QString& filename, int timepoint)
{
	snapshotFilename_ = filename;
	snapshotTimePoint_ = timepoint;
}

bool VideoPlayerWidget::event(QEvent* e)
{
	QEvent::Type type = e->type();

	if (type == QEvent::HoverMove)
	{
		controlVisible_ = 1;
	}
	else if (type == QEvent::Leave)
	{
		controlVisible_ = -100;
	}
	else if (type == QEvent::Enter)
	{
	}

	return QWidget::event(e);
}

void VideoPlayerWidget::mousePressEvent(QMouseEvent* e)
{
}

void VideoPlayerWidget::resizeEvent(QResizeEvent* e)
{
	QSize s = e->size();

	int h = controlWidget_->sizeHint().height();

	controlWidget_->resize(s.width(), h);
	controlWidget_->move(0, s.height());
}

void VideoPlayerWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
}

void VideoPlayerWidget::onSeek(int n)
{
	player_->setTime(n);
}

void VideoPlayerWidget::onPlayPause()
{
	if (player_->state() == Vlc::Playing) {
		player_->pause();
	}
	else if (player_->state() == Vlc::Paused) {
		player_->play();
	}
	else if (player_->state() == Vlc::Ended) {
		player_->stop();
		player_->play();
	}
	else if (player_->state() == Vlc::Stopped) {
		player_->play();
	}
}

void VideoPlayerWidget::onStop()
{
	player_->stop();
	control_.seekBar->setMaximum(1);
	control_.seekBar->setValue(0);
}

void VideoPlayerWidget::onMute(bool mute)
{
	player_->audio()->setMute(mute);
}

void VideoPlayerWidget::onVolume(int n)
{
	if (n == 0) {
		control_.muteButton->setChecked(true);
	}
	else if (n > 0) {
		control_.muteButton->setChecked(false);
	}

	player_->audio()->setVolume(n);
}

void VideoPlayerWidget::onLengthChanged(int n)
{
	control_.seekBar->setValue(0);
	control_.seekBar->setMaximum(n);

	control_.infoLabel->setText("--:--:--");
}

void VideoPlayerWidget::onTimeChanged(int n)
{
	control_.seekBar->blockSignals(true);
	control_.seekBar->setValue(n);
	control_.seekBar->blockSignals(false);

	control_.infoLabel->setText(QTime(0, 0).addMSecs(n).toString("hh:mm:ss"));
}

void VideoPlayerWidget::onTimeout()
{
	if (player_->state() == Vlc::Playing || player_->state() == Vlc::Paused || player_->state() == Vlc::Ended)
	{
		if (snapshotTimePoint_ >= 0 && player_->time() >= snapshotTimePoint_) {
			if (player_->video()->takeSnapshot(snapshotFilename_)) {
				snapshotTimePoint_ = -1;
				Q_EMIT snapshot(snapshotFilename_);
			}
		}
	}

	if (controlVisible_ > 0) {
		int y = controlWidget_->y();
		int h = controlWidget_->sizeHint().height();
		if (y > height() - h) {
			controlWidget_->move(0, y - std::min(h / 20, y - height() + h));
		}
	}
	else if (controlVisible_ == 0) {
		int y = controlWidget_->y();
		int h = controlWidget_->sizeHint().height();
		if (y < height()) {
			controlWidget_->move(0, y + std::min(h / 20, height() - y));
		}
	}
	else {
		++controlVisible_;
	}
}

