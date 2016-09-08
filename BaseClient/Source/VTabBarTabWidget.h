#ifndef VTABBARTABWIDGET_HEADER_
#define VTABBARTABWIDGET_HEADER_

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

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
	QLabel* label_;
	QLabel* superscript_;

private:
	QBoxLayout* layout_;
};

#endif // VTABBARTABWIDGET_HEADER_

