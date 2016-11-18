#ifndef CATEGORYSELECTORWIDGET_HEADER_
#define CATEGORYSELECTORWIDGET_HEADER_

#include <QWidget>
#include <QStringList>
#include <QBoxLayout>

class CategorySelectorWidget : public QWidget {
private:
	Q_OBJECT

public:
	CategorySelectorWidget(QWidget* parent = 0);
	~CategorySelectorWidget();

	void setCategories(const QStringList&);

protected:
	virtual void mousePressEvent(QMouseEvent* e);

private:
	QBoxLayout* layout_;
};

#endif // CATEGORYSELECTORWIDGET_HEADER_

