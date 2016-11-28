#ifndef LABELSELECTORWIDGET_HEADER_
#define LABELSELECTORWIDGET_HEADER_

#include <QWidget>
#include <QStringList>
#include <QSet>
#include <QBoxLayout>

class LabelSelectorWidget : public QWidget {
private:
	Q_OBJECT

public:
	LabelSelectorWidget(QWidget* parent = 0);
	~LabelSelectorWidget();

	void setLabels(const QStringList&);

	void setSelectedLabels(const QStringList&);
	QStringList getSelectedLabels();

Q_SIGNALS:
	void clicked(const QString&, bool);

protected:
	virtual void mousePressEvent(QMouseEvent* e);

private:
	QBoxLayout* layout_;
	QStringList list_;
	QSet<QString> set_;
};

#endif // LABELSELECTORWIDGET_HEADER_

