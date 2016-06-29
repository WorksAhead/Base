#ifndef PAGE_HEADER_
#define PAGE_HEADER_

#include "ui_Page.h"

#include <RpcSession.h>

#include "Context.h"

class Page : public QWidget {
private:
	Q_OBJECT

public:
	Page(ContextPtr context, const QString& name, QWidget* parent = 0);
	~Page();

protected:
	virtual void showEvent(QShowEvent*);

	virtual void paintEvent(QPaintEvent*);

	void foo();

private:
	ContextPtr context_;
	QString name_;

	Ui::Page ui_;
	bool firstShow_;
};

#endif // PAGE_HEADER_

