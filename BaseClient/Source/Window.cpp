#include "Window.h"

#include <QPainter>
#include <QApplication>
#include <QtMath>

#define STATE_BUTTON_MINIMIZE 0
#define STATE_BUTTON_MAXIMIZE 1
#define STATE_BUTTON_RESTORE 2
#define STATE_BUTTON_CLOSE 3

#define SIZEWINDOWFRAME 9

Window::Window()
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_Hover);

	iconWidget_ = new QLabel;
	titleWidget_ = new QLabel;
	decoratorWidget_ = new QWidget;
	centralWidget_ = new QWidget;

	minimizeButton_ = createStateButton(STATE_BUTTON_MINIMIZE);
	maximizeOrRestoreButton_ = createStateButton(STATE_BUTTON_MAXIMIZE);
	closeButton_ = createStateButton(STATE_BUTTON_CLOSE);

	topLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	topLayout_->setContentsMargins(10, 0, 0, 0);
	topLayout_->setSpacing(5);
	topLayout_->addWidget(iconWidget_, 0);
	topLayout_->addWidget(titleWidget_, 1);
	topLayout_->addWidget(decoratorWidget_, 0);
	topLayout_->addWidget(minimizeButton_, 0, Qt::AlignTop);
	topLayout_->addWidget(maximizeOrRestoreButton_, 0, Qt::AlignTop);
	topLayout_->addWidget(closeButton_, 0, Qt::AlignTop);

	topWidget_ = new QWidget;
	topWidget_->setAutoFillBackground(true);
	topWidget_->setLayout(topLayout_);
	topWidget_->setFixedHeight(30);

	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setMargin(0);
	layout_->setSpacing(0);
	layout_->addWidget(topWidget_);
	layout_->addWidget(centralWidget_);

	setLayout(layout_);

	QObject::connect(minimizeButton_, &QPushButton::clicked, this, &Window::showMinimized);
	QObject::connect(maximizeOrRestoreButton_, &QPushButton::clicked, this, &Window::showMaximized);
	QObject::connect(closeButton_, &QPushButton::clicked, this, &Window::close);

	setFrame(SIZEWINDOWFRAME);

	cursorChanged_ = false;
	dragging_ = false;
}

Window::~Window()
{
}

void Window::setWindowIcon(const QIcon& icon)
{
	iconWidget_->setPixmap(icon.pixmap(QSize(20, 20)));
}

void Window::setWindowTitle(const QString& title)
{
	titleWidget_->setText(title);
	QWidget::setWindowTitle(title);
}

QString Window::windowTitle() const
{
	return titleWidget_->text();
}

void Window::setCentralWidget(QWidget* centralWidget, QWidget** outOldCentralWidget)
{
	QWidget* oldCentralWidget = centralWidget_;

	QLayoutItem* li = layout_->replaceWidget(oldCentralWidget, centralWidget);
	delete li;

	centralWidget->setVisible(true);

	centralWidget_ = centralWidget;

	if (outOldCentralWidget) {
		*outOldCentralWidget = oldCentralWidget;
	}
	else {
		delete oldCentralWidget;
	}

	setAttribute(Qt::WA_DeleteOnClose, centralWidget->testAttribute(Qt::WA_DeleteOnClose));
}

QWidget* Window::centralWidget() const
{
	return centralWidget_;
}

void Window::setDecoratorWidget(QWidget* decoratorWidget, QWidget** outOldDecoratorWidget)
{
	QWidget* oldDecoratorWidget = decoratorWidget_;

	QLayoutItem* li = topLayout_->replaceWidget(oldDecoratorWidget, decoratorWidget);
	delete li;

	decoratorWidget->setVisible(true);

	decoratorWidget_ = decoratorWidget;

	if (outOldDecoratorWidget) {
		*outOldDecoratorWidget = oldDecoratorWidget;
	}
	else {
		delete oldDecoratorWidget;
	}
}

QWidget* Window::decoratorWidget() const
{
	return decoratorWidget_;
}

QRect Window::frameGeometry() const
{
	return QWidget::geometry();
}

const QRect& Window::geometry() const
{
	static QRect g;
	g = frameGeometry();
	g.adjust(frame_, frame_, -frame_, -frame_);
	return g;
}

int Window::x() const
{
	return QWidget::x() + frame_;
}

int Window::y() const
{
	return QWidget::y() + frame_;
}

QPoint Window::pos() const
{
	return QPoint(x(), y());
}

QSize Window::frameSize() const
{
	return QWidget::size();
}

int Window::frameWidth() const
{
	return QWidget::width();
}

int Window::frameHeight() const
{
	return QWidget::height();
}

QSize Window::size() const
{
	return frameSize() - QSize(frame_ * 2, frame_ * 2);
}

int Window::width() const
{
	return QWidget::width() - frame_ * 2;
}

int Window::height() const
{
	return QWidget::height() - frame_ * 2;
}

QRect Window::rect() const
{
	return QRect(0, 0, width(), height());
}

QSize Window::minimumSize() const
{
	return QSize(minimumWidth(), minimumHeight());
}

QSize Window::maximumSize() const
{
	return QSize(maximumWidth(), maximumHeight());
}

int Window::minimumWidth() const
{
	const int frame = SIZEWINDOWFRAME;
	return QWidget::minimumWidth() - frame;
}

int Window::minimumHeight() const
{
	const int frame = SIZEWINDOWFRAME;
	return QWidget::minimumHeight() - frame;
}

int Window::maximumWidth() const
{
	const int frame = SIZEWINDOWFRAME;
	return QWidget::maximumWidth() - frame;
}

int Window::maximumHeight() const
{
	const int frame = SIZEWINDOWFRAME;
	return QWidget::maximumHeight() - frame;
}

void Window::setMinimumSize(const QSize& s)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMinimumSize(s + QSize(frame * 2, frame * 2));
}

void Window::setMinimumSize(int minw, int minh)
{
	setMinimumSize(QSize(minw, minh));
}

void Window::setMaximumSize(const QSize& s)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMaximumSize(s + QSize(frame * 2, frame * 2));
}

void Window::setMaximumSize(int maxw, int maxh)
{
	setMaximumSize(QSize(maxw, maxh));
}

void Window::setMinimumWidth(int minw)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMinimumWidth(minw + frame * 2);
}

void Window::setMinimumHeight(int minh)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMinimumHeight(minh + frame * 2);
}

void Window::setMaximumWidth(int maxw)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMaximumWidth(maxw + frame * 2);
}

void Window::setMaximumHeight(int maxh)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setMaximumHeight(maxh + frame * 2);
}

void Window::setFixedSize(const QSize& s)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setFixedSize(s + QSize(frame * 2, frame * 2));
}

void Window::setFixedSize(int w, int h)
{
	setFixedSize(QSize(w, h));
}

void Window::setFixedWidth(int w)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setFixedWidth(w + frame * 2);
}

void Window::setFixedHeight(int h)
{
	const int frame = SIZEWINDOWFRAME;
	QWidget::setFixedWidth(h + frame * 2);
}

void Window::move(int x, int y)
{
	move(QPoint(x, y));
}

void Window::move(const QPoint& p)
{
	if (isMaximized() || isFullScreen()) {
		showNormal();
	}

	QWidget::move(p - origin());
}

void Window::resize(int w, int h)
{
	resize(QSize(w, h));
}

void Window::resize(const QSize& s)
{
	if (isMaximized() || isFullScreen()) {
		showNormal();
	}

	QWidget::resize(s + QSize(frame_ * 2, frame_ * 2));
}

QPoint Window::origin() const
{
	return QPoint(frame_, frame_);
}

bool Window::event(QEvent* e)
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
	else if (type == QEvent::WindowStateChange)
	{
		onStateChange();
	}

	return QWidget::event(e);
}

void Window::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		dragDetector_.update(e->x(), e->y(), QWidget::width(), QWidget::height());
		dragStartPos_ = e->pos();
		dragging_ = true;
	}
}

void Window::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		dragDetector_.reset();
		dragStartPos_ = QPoint(0, 0);
		dragging_ = false;
	}
}

void Window::mouseMoveEvent(QMouseEvent* e)
{
	if (dragging_) {
		onDrag(e->pos());
	}
	else {
		onHover(e->pos());
	}
}

void Window::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	if (!isMaximized() && !isFullScreen())
	{
		QColor color = QColor(40, 40, 40);

		QRect border(QPoint(0, 0), frameSize());
		border.adjust(frame_ - 1, frame_ - 1, -frame_, -frame_);

		for (int i = 0; i < frame_; ++i)
		{
			color.setAlphaF(0.005);
			painter.setPen(color);

			QPainterPath path;
			path.addRect(border);
			painter.drawPath(path);

			border.adjust(-1, -1, 1, 1);
		}
	}
}

void Window::setFrame(int frame)
{
	layout_->setMargin(frame);
	hoverDetector_.setBorder(frame);
	dragDetector_.setBorder(frame);
	frame_ = frame;
}

void Window::onStateChange()
{
	Qt::WindowStates state = windowState();

	if (state == Qt::WindowMaximized || state == Qt::WindowFullScreen)
	{
		QPushButton* restoreButton = createStateButton(STATE_BUTTON_RESTORE);
		QLayoutItem* li = topLayout_->replaceWidget(maximizeOrRestoreButton_, restoreButton);
		delete li->widget();
		delete li;

		QObject::connect(restoreButton, &QPushButton::clicked, this, &Window::showNormal);
		maximizeOrRestoreButton_ = restoreButton;

		setFrame(0);
	}
	else
	{
		QPushButton* maximizeButton = createStateButton(STATE_BUTTON_MAXIMIZE);
		QLayoutItem* li = topLayout_->replaceWidget(maximizeOrRestoreButton_, maximizeButton);
		delete li->widget();
		delete li;

		QObject::connect(maximizeButton, &QPushButton::clicked, this, &Window::showMaximized);
		maximizeOrRestoreButton_ = maximizeButton;

		setFrame(SIZEWINDOWFRAME);
	}
}

void Window::onDrag(const QPoint& pos)
{
	if (isMaximized() || isFullScreen()) {
		return;
	}

	const QRect& rect = frameGeometry();

	int left = rect.left();
	int top = rect.top();
	int right = rect.right();
	int bottom = rect.bottom();

	rect.getCoords(&left, &top, &right, &bottom);

	if (dragDetector_.onTopLeftEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
		left = left + pos.x() - dragDetector_.xOffset();
	}
	else if (dragDetector_.onBottomLeftEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
		left = left + pos.x() - dragDetector_.xOffset();
	}
	else if (dragDetector_.onTopRightEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
		right = left + pos.x() + dragDetector_.xOffset();
	}
	else if (dragDetector_.onBottomRightEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
		right = left + pos.x() + dragDetector_.xOffset();
	}
	else if (dragDetector_.onLeftEdge()) {
		left = left + pos.x() - dragDetector_.xOffset();
	}
	else if (dragDetector_.onRightEdge()) {
		right = left + pos.x() + dragDetector_.xOffset();
	}
	else if (dragDetector_.onTopEdge()) {
		top = top + pos.y() - dragDetector_.yOffset();
	}
	else if (dragDetector_.onBottomEdge()) {
		bottom = top + pos.y() + dragDetector_.yOffset();
	}
	else if (titleWidget_->geometry().contains(dragStartPos_)) {
		left = left + pos.x() - dragStartPos_.x();
		right = right + pos.x() - dragStartPos_.x();
		top = top + pos.y() - dragStartPos_.y();
		bottom = bottom + pos.y() - dragStartPos_.y();
	}

	QRect newRect(QPoint(left, top), QPoint(right, bottom));

	if (newRect.width() > QWidget::maximumWidth() || newRect.width() < QWidget::minimumWidth()) {
		if (left != rect.left()) {
			newRect.setLeft(rect.left());
		}
		else {
			newRect.setRight(rect.right());
		}
	}

	if (newRect.height() > QWidget::maximumHeight() || newRect.height() < QWidget::minimumHeight()) {
		if (top != rect.top()) {
			newRect.setTop(rect.top());
		}
		else {
			newRect.setBottom(rect.bottom());
		}
	}

	setGeometry(newRect);
}

void Window::onHover(const QPoint& pos)
{
	if (!isMaximized() && !isFullScreen())
	{
		hoverDetector_.update(pos.x(), pos.y(), QWidget::width(), QWidget::height());

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
	}

	if (cursorChanged_) {
		unsetCursor();
		cursorChanged_ = false;
	}
}

QPushButton* Window::createStateButton(int type)
{
	QPushButton* button = new QPushButton;

	button->setFlat(true);
	button->setFixedSize(QSize(45, 26));

	switch (type)
	{
	case STATE_BUTTON_MINIMIZE:
		button->setToolTip(tr("Minimize"));
		button->setIcon(QIcon(":/Icons/WindowMinimize.png"));
		break;
	case STATE_BUTTON_MAXIMIZE:
		button->setToolTip(tr("Maximize"));
		button->setIcon(QIcon(":/Icons/WindowMaximize.png"));
		break;
	case STATE_BUTTON_RESTORE:
		button->setToolTip(tr("Restore"));
		button->setIcon(QIcon(":/Icons/WindowRestore.png"));
		break;
	case STATE_BUTTON_CLOSE:
		button->setToolTip(tr("Close"));
		button->setIcon(QIcon(":/Icons/WindowClose.png"));
		break;
	};

	button->setIconSize(QSize(20, 20));
	
	return button;
}

