#ifndef PROGRESSBAR_HEADER_
#define PROGRESSBAR_HEADER_

#include <QWidget>

class ProgressBar : public QWidget {
private:
	Q_OBJECT

public:
	explicit ProgressBar(QWidget* parent = 0);
	~ProgressBar();

Q_SIGNALS:
	void clicked(int);

public Q_SLOTS:
	void setRange(int minimum, int maximum);
	void setMinimum(int minimum);
	void setMaximum(int maximum);
	void setValue(int value);

protected:
	virtual void mousePressEvent(QMouseEvent*);

	virtual void paintEvent(QPaintEvent*);

private:
	int minimum_;
	int maximum_;
	int value_;
};

#endif // PROGRESSBAR_HEADER_

