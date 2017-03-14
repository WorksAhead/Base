#ifndef SUBMITEXTRADIALOG_HEADER_
#define SUBMITEXTRADIALOG_HEADER_

#include "Context.h"

#include "ui_SubmitExtraDialog.h"

#include <QString>
#include <QPixmap>

class SubmitExtraDialog : public QDialog {
private:
	Q_OBJECT

public:
	explicit SubmitExtraDialog(ContextPtr context, QWidget* parent = 0);
	~SubmitExtraDialog();

	void switchToEditMode();

	void setTitle(const QString& title);
	void setCategory(const QString& category);
	void setSetup(const QString& setup);
	void setInfo(const QString& info);

	QString getTitle() const;
	QString getLocation() const;
	QString getCategory() const;
	QString getSetup() const;
	QString getCoverImage() const;
	QString getInfo() const;

private Q_SLOTS:
	void editCategory();
	void selectLocation();
	void setCover();
	void submit();

private:
	QPixmap getImage(const QSize& ratio);
	QString getOpenImageFileName();

private:
	ContextPtr context_;
	Ui::SubmitExtraDialog ui_;
	QString coverImage_;
	bool editMode_;
};

#endif // SUBMITEXTRADIALOG_HEADER_

