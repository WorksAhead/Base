#ifndef LOGINDIALOG_HEADER_
#define LOGINDIALOG_HEADER_

#include <ui_LoginDialog.h>

#include <RpcStart.h>
#include <RpcSession.h>

class LoginDialog : public QDialog {
private:
	Q_OBJECT

public:
	LoginDialog(Rpc::StartPrx, QWidget* parent = 0);
	~LoginDialog();

	Rpc::SessionPrx session();

private Q_SLOTS:
	void onLogin();
	void onSignup();
	void onReset();

private:
	Ui::LoginDialog ui_;
	Rpc::StartPrx startPrx_;
	Rpc::SessionPrx sessionPrx_;
	QString password_;
};

#endif // LOGINDIALOG_HEADER_

