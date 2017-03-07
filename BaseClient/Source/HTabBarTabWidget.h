#ifndef HTABBARTABWIDGET_HEADER_
#define HTABBARTABWIDGET_HEADER_

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QString>
#include <QFont>

class HTabBarTabWidget : public QWidget {
private:
	Q_OBJECT

public:
	HTabBarTabWidget(QWidget* parent = 0);
	~HTabBarTabWidget();

	void setLabel(const QString& text);
	void setLabelFont(const QFont& font);

	void setActive(bool);

	void addNotification();
	void clearNotification();

protected:
	virtual void paintEvent(QPaintEvent*);

private:
	QString text_;

	QFont font_;

	int notification_;
};

#endif // HTABBARTABWIDGET_HEADER_

