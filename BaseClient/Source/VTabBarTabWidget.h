#ifndef VTABBARTABWIDGET_HEADER_
#define VTABBARTABWIDGET_HEADER_

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QString>
#include <QFont>

class VTabBarTabWidget : public QWidget {
private:
	Q_OBJECT

public:
	VTabBarTabWidget(QWidget* parent = 0);
	~VTabBarTabWidget();

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

#endif // VTABBARTABWIDGET_HEADER_

