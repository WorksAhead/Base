#ifndef FLOWLAYOUT_HEADER_
#define FLOWLAYOUT_HEADER_

#include <QLayout>
#include <QRect>
#include <QStyle>

class FlowLayout : public QLayout {
private:
	Q_OBJECT

public:
	explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
	explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
	~FlowLayout();

	void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;
	void addWidget(QWidget*);
	void insertWidget(int index, QWidget*);
	int horizontalSpacing() const;
	int verticalSpacing() const;
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
	bool hasHeightForWidth() const Q_DECL_OVERRIDE;
	int heightForWidth(int) const Q_DECL_OVERRIDE;
	int count() const Q_DECL_OVERRIDE;
	QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
	QSize minimumSize() const Q_DECL_OVERRIDE;
	void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;
	QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;

private:
	int doLayout(const QRect& rect, bool testOnly) const;
	int smartSpacing(QStyle::PixelMetric pm) const;

	QList<QLayoutItem*> itemList;
	int m_hSpace;
	int m_vSpace;
};

#endif // FLOWLAYOUT_HEADER_

