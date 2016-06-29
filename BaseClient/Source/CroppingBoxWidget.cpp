#include "CroppingBoxWidget.h"

#include <QPainter>
#include <QHoverEvent>
#include <QMouseEvent>

CroppingBoxWidget::CroppingBoxWidget(QWidget* parent) : QWidget(parent)
{
	setAttribute(Qt::WA_Hover);
	setAttribute(Qt::WA_TranslucentBackground);

	setMinimumSize(QSize(10, 10));

	ratio_ = 0.0;

	hoverDetector_.setBorder(5);
	dragDetector_.setBorder(5);
}

CroppingBoxWidget::~CroppingBoxWidget()
{
}

void CroppingBoxWidget::setRatio(const QSize& size)
{
	if (size.isEmpty()) {
		ratio_ = 0.0f;
	}
	else {
		ratio_ = (float)((double)size.width() / (double)size.height());
	}
}

void CroppingBoxWidget::setArea(const QRect& area)
{
	area_ = area;
}

bool CroppingBoxWidget::event(QEvent* e)
{
	QEvent::Type type = e->type();

	if (type == QEvent::HoverMove)
	{
		if (dragging_ == false) {
			const QPoint& pos = static_cast<QHoverEvent*>(e)->pos();
			onHover(pos);
		}
	}
	else if (type == QEvent::Leave)
	{
		if (cursorChanged_) {
			unsetCursor();
		}
	}
	else if (type == QEvent::Enter)
	{
	}

	return QWidget::event(e);
}

void CroppingBoxWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		dragDetector_.update(e->pos().x(), e->pos().y(), width(), height());
		dragStartPos_ = e->pos();
		dragging_ = true;
	}
}

void CroppingBoxWidget::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		dragDetector_.reset();
		dragStartPos_ = QPoint(0, 0);
		dragging_ = false;
	}
}

void CroppingBoxWidget::mouseMoveEvent(QMouseEvent* e)
{
	if (dragging_) {
		onDrag(e->pos());
	}
	else {
		onHover(e->pos());
	}
}

void CroppingBoxWidget::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	QRect outline = rect().adjusted(0, 0, -1, -1);

	QPen pen;
	pen.setWidth(1);
	pen.setColor(QColor(246, 96, 0));

	painter.setPen(pen);

	for (int i = 0; i < 3; ++i) {
		painter.drawRect(outline);
		outline.adjust(1, 1, -1, -1);
	}
}

void CroppingBoxWidget::onDrag(const QPoint& pos)
{
	int left, top, right, bottom;
	geometry().getCoords(&left, &top, &right, &bottom);

	bool leftChanged = false;
	bool rightChanged = false;
	bool topChanged = false;
	bool bottomChanged = false;
	bool moving = false;

	if (dragDetector_.onTopLeftEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
		left = left + pos.x() - dragDetector_.xOffset();
		topChanged = leftChanged = true;
	}
	else if (dragDetector_.onBottomLeftEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
		left = left + pos.x() - dragDetector_.xOffset();
		bottomChanged = leftChanged = true;
	}
	else if (dragDetector_.onTopRightEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
		right = left + pos.x() + dragDetector_.xOffset();
		topChanged = rightChanged = true;
	}
	else if (dragDetector_.onBottomRightEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
		right = left + pos.x() + dragDetector_.xOffset();
		bottomChanged = rightChanged = true;
	}
	else if (dragDetector_.onLeftEdge()) {
		left = left + pos.x() - dragDetector_.xOffset();
		leftChanged = true;
	}
	else if (dragDetector_.onRightEdge()) {
		right = left + pos.x() + dragDetector_.xOffset();
		rightChanged = true;
	}
	else if (dragDetector_.onTopEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
		topChanged = true;
	}
	else if (dragDetector_.onBottomEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
		bottomChanged = true;
	}
	else {
		left = left + pos.x() - dragStartPos_.x();
		right = right + pos.x() - dragStartPos_.x();
		top = top + pos.y() - dragStartPos_.y();
		bottom = bottom + pos.y() - dragStartPos_.y();
		moving = true;
	}

	QRect newGeometry(QPoint(left, top), QPoint(right, bottom));

	const int maximumWidth = QWidget::maximumWidth();
	const int minimumWidth = QWidget::minimumWidth();
	const int maximumHeight = QWidget::maximumHeight();
	const int minimumHeight = QWidget::minimumHeight();

	if (newGeometry.width() > maximumWidth) {
		if (leftChanged) {
			newGeometry.setLeft(newGeometry.right() - maximumWidth + 1);
		}
		else {
			newGeometry.setRight(newGeometry.left() + maximumWidth - 1);
		}
	}
	else if (newGeometry.width() < minimumWidth) {
		if (leftChanged) {
			newGeometry.setLeft(newGeometry.right() - minimumWidth + 1);
		}
		else {
			newGeometry.setRight(newGeometry.left() + minimumWidth - 1);
		}
	}

	if (newGeometry.height() > maximumHeight) {
		if (topChanged) {
			newGeometry.setTop(newGeometry.bottom() - maximumHeight + 1);
		}
		else {
			newGeometry.setBottom(newGeometry.top() + maximumHeight - 1);
		}
	}
	else if (newGeometry.height() < minimumHeight) {
		if (topChanged) {
			newGeometry.setTop(newGeometry.bottom() - minimumHeight + 1);
		}
		else {
			newGeometry.setBottom(newGeometry.top() + minimumHeight - 1);
		}
	}

	if (!moving && ratio_ != 0.0)
	{
		if (leftChanged && topChanged) {
			newGeometry.setTop(newGeometry.bottom() - newGeometry.width() / ratio_ + 1);
		}
		else if (rightChanged && topChanged) {
			newGeometry.setTop(newGeometry.bottom() - newGeometry.width() / ratio_ + 1);
		}
		else if (leftChanged || rightChanged) {
			newGeometry.setBottom(newGeometry.top() + newGeometry.width() / ratio_ - 1);
		}
		else if (topChanged || bottomChanged) {
			newGeometry.setRight(newGeometry.left() + newGeometry.height() * ratio_ - 1);
		}
	}

	if (area_.isValid())
	{
		if (newGeometry.left() < area_.left())
		{
			if (moving) {
				newGeometry.moveLeft(area_.left());
			}
			else {
				newGeometry.setLeft(area_.left());
				if (ratio_ != 0.0) {
					if (topChanged) {
						newGeometry.setTop(newGeometry.bottom() - newGeometry.width() / ratio_ + 1);
					}
					else {
						newGeometry.setBottom(newGeometry.top() + newGeometry.width() / ratio_ - 1);
					}
				}
			}
		}
		if (newGeometry.right() > area_.right())
		{
			if (moving) {
				newGeometry.moveRight(area_.right());
			}
			else {
				newGeometry.setRight(area_.right());
				if (ratio_ != 0.0) {
					if (topChanged) {
						newGeometry.setTop(newGeometry.bottom() - newGeometry.width() / ratio_ + 1);
					}
					else {
						newGeometry.setBottom(newGeometry.top() + newGeometry.width() / ratio_ - 1);
					}
				}
			}
		}
		if (newGeometry.top() < area_.top())
		{
			if (moving) {
				newGeometry.moveTop(area_.top());
			}
			else {
				newGeometry.setTop(area_.top());
				if (ratio_ != 0.0) {
					if (leftChanged) {
						newGeometry.setLeft(newGeometry.right() - newGeometry.height() * ratio_ + 1);
					}
					else {
						newGeometry.setRight(newGeometry.left() + newGeometry.height() * ratio_ - 1);
					}
				}
			}
		}
		if (newGeometry.bottom() > area_.bottom()) {
			if (moving) {
				newGeometry.moveBottom(area_.bottom());
			}
			else {
				newGeometry.setBottom(area_.bottom());
				if (ratio_ != 0.0) {
					if (leftChanged) {
						newGeometry.setLeft(newGeometry.right() - newGeometry.height() * ratio_ + 1);
					}
					else {
						newGeometry.setRight(newGeometry.left() + newGeometry.height() * ratio_ - 1);
					}
				}
			}
		}
	}

	setGeometry(newGeometry);
}

void CroppingBoxWidget::onHover(const QPoint& pos)
{
	hoverDetector_.update(pos.x(), pos.y(), width(), height());

	if (hoverDetector_.onTopLeftEdge() || hoverDetector_.onBottomRightEdge()) {
		setCursor(Qt::SizeFDiagCursor);
		cursorChanged_ = true;
		return;
	}
	else if (hoverDetector_.onTopRightEdge() || hoverDetector_.onBottomLeftEdge()) {
		setCursor(Qt::SizeBDiagCursor);
		cursorChanged_ = true;
		return;
	}
	else if (hoverDetector_.onLeftEdge() || hoverDetector_.onRightEdge()) {
		setCursor(Qt::SizeHorCursor);
		cursorChanged_ = true;
		return;
	}
	else if (hoverDetector_.onTopEdge() || hoverDetector_.onBottomEdge()) {
		setCursor(Qt::SizeVerCursor);
		cursorChanged_ = true;
		return;
	}

	if (cursorChanged_) {
		unsetCursor();
		cursorChanged_ = false;
	}
}

