#ifndef STACKEDWIDGET_HEADER_
#define STACKEDWIDGET_HEADER_

#include <QStackedWidget>

class StackedWidget : public QStackedWidget {
private:
	Q_OBJECT

public:
	explicit StackedWidget(QWidget* parent = 0);
	~StackedWidget();

	void setAspectRatio(float);

protected:
	virtual void resizeEvent(QResizeEvent*);

private:
	float aspectRatio_;
};

#endif // STACKEDWIDGET_HEADER_

