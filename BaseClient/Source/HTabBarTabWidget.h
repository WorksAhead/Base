#ifndef HTABBARTABWIDGET_HEADER_
#define HTABBARTABWIDGET_HEADER_

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

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
	QLabel* label_;
	QLabel* superscript_;

private:
	QBoxLayout* layout_;
};

#endif // HTABBARTABWIDGET_HEADER_

