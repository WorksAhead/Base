#ifndef LOGINDIALOG_HEADER_
#define LOGINDIALOG_HEADER_

#include <ui_LoginDialog.h>

#include <RpcStart.h>
#include <RpcSession.h>

#include <string>

class LoginDialog : public QDialog {
private:
	Q_OBJECT

public:
	LoginDialog(const QString& workPath, Rpc::StartPrx, QWidget* parent = 0);
	~LoginDialog();

	Rpc::SessionPrx session();

private Q_SLOTS:
	void onLogin();
	void onSignup();
	void onReset();

private:
	Ui::LoginDialog ui_;
	std::string workPath_;
	Rpc::StartPrx startPrx_;
	Rpc::SessionPrx sessionPrx_;
	QString password_;
};

#endif // LOGINDIALOG_HEADER_

