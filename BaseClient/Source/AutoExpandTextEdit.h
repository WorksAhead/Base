#ifndef AUTOEXPANDTEXTEDIT_HEADER_
#define AUTOEXPANDTEXTEDIT_HEADER_

#include <QPlainTextEdit>

class AutoExpandTextEdit : public QPlainTextEdit {
private:
	Q_OBJECT

public:
	explicit AutoExpandTextEdit(QWidget* parent = 0);
	~AutoExpandTextEdit();

private Q_SLOTS:
	void onTextChanged();

private:
	int lineHeight_;
};

#endif // AUTOEXPANDTEXTEDIT_HEADER_

