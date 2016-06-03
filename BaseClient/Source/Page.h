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

private:
	Ui::Page ui_;
	bool firstShow_;

	ContextPtr context_;

};

#endif // PAGE_HEADER_

