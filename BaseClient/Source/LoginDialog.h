#ifndef LOGINDIALOG_HEADER_
#define LOGINDIALOG_HEADER_

#include <ui_LoginDialog.h>

#include <RpcStart.h>
#include <RpcSession.h>

#include <string>

#include "Security/Rijndael.h"

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

	QByteArray encrypt(const QString& pwd);

private:
	Ui::LoginDialog ui_;
	std::string workPath_;
	Rpc::StartPrx startPrx_;
	Rpc::SessionPrx sessionPrx_;
	QString password_;
	CRijndael rijndael_;
};

#endif // LOGINDIALOG_HEADER_

