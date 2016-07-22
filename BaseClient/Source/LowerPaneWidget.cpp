#include "LowerPaneWidget.h"
#include "ASyncTaskWidget.h"

#include <QPainter>
#include <QMouseEvent>

LowerPaneWidget::LowerPaneWidget(ContextPtr context, QWidget* parent) : QWidget(parent), context_(context)
{
	setAutoFillBackground(true);
	ui_.setupUi(this);

	turnTimer_ = new QTimer(this);
	refreshTimer_ = new QTimer(this);

	refreshTimer_->start(100);

	QObject::connect(turnTimer_, &QTimer::timeout, this, &LowerPaneWidget::onTurn);
}

LowerPaneWidget::~LowerPaneWidget()
{
}

void LowerPaneWidget::addTask(ASyncTaskPtr task)
{
	ASyncTaskWidget* w = new ASyncTaskWidget(task);

	const int index = ui_.taskStackedWidget->addWidget(w);
	ui_.taskStackedWidget->setCurrentIndex(index);

	QObject::connect(refreshTimer_, &QTimer::timeout, w, &ASyncTaskWidget::refresh);

	w->refresh();

	turnTimer_->start(2000);
}

void LowerPaneWidget::onTurn()
{
	const int count = ui_.taskStackedWidget->count();
	if (count == 0) {
		return;
	}

	ASyncTaskWidget* w = qobject_cast<ASyncTaskWidget*>(ui_.taskStackedWidget->currentWidget());
	w->refresh();

	const int state = w->task()->state();

	if (state == ASyncTask::state_cancelled || state == ASyncTask::state_finished) {
		w->deleteLater();
		ui_.taskStackedWidget->removeWidget(w);
		if (ui_.taskStackedWidget->count() == 0) {
			turnTimer_->stop();
		}
		return;
	}

	const int currentIndex = ui_.taskStackedWidget->currentIndex();
	ui_.taskStackedWidget->setCurrentIndex((currentIndex + 1) % count);
}

void LowerPaneWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		if (ui_.taskStackedWidget->count()) {
			QPoint pos = ui_.taskStackedWidget->mapFrom(this, e->pos());
			if (ui_.taskStackedWidget->rect().contains(pos)) {
				context_->showTaskManager();
			}
		}
	}
}

void LowerPaneWidget::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

