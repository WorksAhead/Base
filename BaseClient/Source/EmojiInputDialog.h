#ifndef EMOJIINPUTDIALOG_HEADER_
#define EMOJIINPUTDIALOG_HEADER_

#include <QDialog>
#include <QScrollArea>

class EmojiInputDialog : public QDialog {
private:
	Q_OBJECT

public:
	EmojiInputDialog(QWidget* parent = 0);
	~EmojiInputDialog();

Q_SIGNALS:
	void emojiPressed(QString);

protected:
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent*);

	virtual void showEvent(QShowEvent*);

private:
	QScrollArea* scrollArea_;
};

#endif // EMOJIINPUTDIALOG_HEADER_

