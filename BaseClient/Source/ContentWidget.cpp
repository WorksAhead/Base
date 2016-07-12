#include "ContentWidget.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QLabel>

ContentWidget::ContentWidget(QWidget* parent) : QWidget(parent)
{
	ui_.setupUi(this);

	thumbnailLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	thumbnailLayout_->setMargin(2);
	thumbnailLayout_->setSpacing(5);

	thumbnailWidget_ = new QWidget;
	thumbnailWidget_->setLayout(thumbnailLayout_);
	ui_.thumbnailScrollArea->setWidget(thumbnailWidget_);
}

ContentWidget::~ContentWidget()
{
}

void ContentWidget::setId(const QString& id)
{
	id_ = id;
}

const QString& ContentWidget::id() const
{
	return id_;
}

void ContentWidget::setTitle(const QString& text)
{
	ui_.titleLabel->setText(text);
}

void ContentWidget::setSummary(const QString& text)
{
	ui_.summaryLabel->setText(text);
}

void ContentWidget::setDescription(const QString& text)
{
	ui_.descriptionLabel->setText(text);
}

void ContentWidget::setImage(int index, const QPixmap& pixmap)
{
	QLayoutItem* li = thumbnailLayout_->itemAt(index);
	if (li && li->widget()) {
		QLabel* label = qobject_cast<QLabel*>(li->widget());
		if (label) {
			label->setPixmap(pixmap.scaled(192, 108, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
	}

	screenshots_[index] = pixmap;

	if (index == 0)
	{
		ui_.screenshotViewer->setPixmap(pixmap);
	}
}

void ContentWidget::setImageCount(int count)
{
	while (thumbnailLayout_->count()) {
		QLayoutItem* li = thumbnailLayout_->takeAt(0);
		li->widget()->deleteLater();
		delete li;
	}

	if (count > 0)
	{
		for (int i = 0; i < count; ++i) {
			QLabel* label = new QLabel;
			label->setFixedSize(192, 108);
			label->setText("Loading");
			label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			thumbnailLayout_->addWidget(label);
		}

		thumbnailLayout_->addStretch(1);
	}

	if (count == 0) {
		ui_.screenshotViewer->setVisible(false);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else if (count == 1) {
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(false);
	}
	else {
		ui_.screenshotViewer->setVisible(true);
		ui_.thumbnailScrollArea->setVisible(true);
	}

	ui_.screenshotViewer->setPixmap(QPixmap());

	screenshots_.clear();
	screenshots_.resize(count);
}

void ContentWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		QPoint pos = thumbnailWidget_->mapFrom(this, e->pos());
		if (thumbnailWidget_->rect().contains(pos))
		{
			QWidget* w = thumbnailWidget_->childAt(pos);
			const int index = thumbnailLayout_->indexOf(w);
			if (index >= 0 && index < screenshots_.count())
			{
				ui_.screenshotViewer->setPixmap(screenshots_.at(index));
			}
		}
	}
}

void ContentWidget::resizeEvent(QResizeEvent* e)
{
	ui_.screenshotViewer->setFixedHeight(ui_.screenshotViewer->width() / 1.7777777777);
}

