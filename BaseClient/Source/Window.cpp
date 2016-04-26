#include "Window.h"

#include <QPainter>
#include <QtMath>

#define WINDOW_FRAME_BORDER 8

Window::Window()
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_Hover);

	iconWidget_ = new QLabel;
	titleWidget_ = new QLabel;
	centralWidget_ = new QWidget;

	QSize smallIconSize(20, 20);
	QSize stateButtonSize(45, 26);

	minimizeButton_ = new QPushButton;
	minimizeButton_->setFlat(true);
	minimizeButton_->setFixedSize(stateButtonSize);
	minimizeButton_->setIcon(QIcon(":/Icons/WindowMinimize.png"));
	minimizeButton_->setIconSize(smallIconSize);
	minimizeButton_->setToolTip(tr("Minimize"));

	maximizeButton_ = new QPushButton;
	maximizeButton_->setFlat(true);
	maximizeButton_->setFixedSize(stateButtonSize);
	maximizeButton_->setIcon(QIcon(":/Icons/WindowMaximize.png"));
	maximizeButton_->setIconSize(smallIconSize);
	maximizeButton_->setToolTip(tr("Maximize"));

	restoreButton_ = new QPushButton;
	restoreButton_->setFlat(true);
	restoreButton_->setFixedSize(stateButtonSize);
	restoreButton_->setIcon(QIcon(":/Icons/WindowRestore.png"));
	restoreButton_->setIconSize(smallIconSize);
	restoreButton_->setToolTip(tr("Restore"));

	closeButton_ = new QPushButton;
	closeButton_->setFlat(true);
	closeButton_->setFixedSize(stateButtonSize);
	closeButton_->setIcon(QIcon(":/Icons/WindowClose.png"));
	closeButton_->setIconSize(smallIconSize);
	closeButton_->setToolTip(tr("Close"));

	topLayout_ = new QBoxLayout(QBoxLayout::LeftToRight);
	topLayout_->setContentsMargins(10, 0, 0, 0);
	topLayout_->setSpacing(5);
	topLayout_->addWidget(iconWidget_, 0);
	topLayout_->addWidget(titleWidget_, 1);
	topLayout_->addWidget(minimizeButton_, 0, Qt::AlignTop);
	topLayout_->addWidget(maximizeButton_, 0, Qt::AlignTop);
	topLayout_->addWidget(restoreButton_, 0, Qt::AlignTop);
	topLayout_->addWidget(closeButton_, 0, Qt::AlignTop);

	topWidget_ = new QWidget;
	topWidget_->setAutoFillBackground(true);
	topWidget_->setLayout(topLayout_);
	topWidget_->setFixedHeight(30);

	const int frame = WINDOW_FRAME_BORDER;

	layout_ = new QBoxLayout(QBoxLayout::TopToBottom);
	layout_->setMargin(0);
	layout_->setSpacing(0);
	layout_->addWidget(topWidget_);
	layout_->addWidget(centralWidget_);

	setLayout(layout_);

	QObject::connect(minimizeButton_, &QPushButton::clicked, this, &Window::showMinimized);
	QObject::connect(maximizeButton_, &QPushButton::clicked, this, &Window::showMaximized);
	QObject::connect(restoreButton_, &QPushButton::clicked, this, &Window::showNormal);
	QObject::connect(closeButton_, &QPushButton::clicked, this, &Window::close);

	setFrame(WINDOW_FRAME_BORDER);

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

void Window::setCentralWidget(QWidget* centralWidget)
{
	QLayoutItem* li = layout_->replaceWidget(centralWidget_, centralWidget);
	if (li) {
		centralWidget_->hide();
		centralWidget_->deleteLater();
		centralWidget_ = centralWidget;
		delete li;
	}
}

QWidget* Window::centralWidget() const
{
	return centralWidget_;
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
	const int frame = WINDOW_FRAME_BORDER;
	return QWidget::minimumWidth() - frame;
}

int Window::minimumHeight() const
{
	const int frame = WINDOW_FRAME_BORDER;
	return QWidget::minimumHeight() - frame;
}

int Window::maximumWidth() const
{
	const int frame = WINDOW_FRAME_BORDER;
	return QWidget::maximumWidth() - frame;
}

int Window::maximumHeight() const
{
	const int frame = WINDOW_FRAME_BORDER;
	return QWidget::maximumHeight() - frame;
}

void Window::setMinimumSize(const QSize& s)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMinimumSize(s + QSize(frame * 2, frame * 2));
}

void Window::setMinimumSize(int minw, int minh)
{
	setMinimumSize(QSize(minw, minh));
}

void Window::setMaximumSize(const QSize& s)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMaximumSize(s + QSize(frame * 2, frame * 2));
}

void Window::setMaximumSize(int maxw, int maxh)
{
	setMaximumSize(QSize(maxw, maxh));
}

void Window::setMinimumWidth(int minw)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMinimumWidth(minw + frame * 2);
}

void Window::setMinimumHeight(int minh)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMinimumHeight(minh + frame * 2);
}

void Window::setMaximumWidth(int maxw)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMaximumWidth(maxw + frame * 2);
}

void Window::setMaximumHeight(int maxh)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setMaximumHeight(maxh + frame * 2);
}

void Window::setFixedSize(const QSize& s)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setFixedSize(s + QSize(frame * 2, frame * 2));
}

void Window::setFixedSize(int w, int h)
{
	setFixedSize(QSize(w, h));
}

void Window::setFixedWidth(int w)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setFixedWidth(w + frame * 2);
}

void Window::setFixedHeight(int h)
{
	const int frame = WINDOW_FRAME_BORDER;
	QWidget::setFixedWidth(h + frame * 2);
}

void Window::show()
{
	showNormal();
}

void Window::showMinimized()
{
	QWidget::showMinimized();
}

void Window::showMaximized()
{
	setFrame(0);

	topWidget_->setVisible(true);
	maximizeButton_->setVisible(false);
	restoreButton_->setVisible(true);

	QWidget::showMaximized();
}

void Window::showFullScreen()
{
}

void Window::showNormal()
{
	setFrame(WINDOW_FRAME_BORDER);

	topWidget_->setVisible(true);
	maximizeButton_->setVisible(true);
	restoreButton_->setVisible(false);

	QWidget::showNormal();
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

