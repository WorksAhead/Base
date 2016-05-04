#include "LoginDialog.h"

#include <ErrorMessage.h>

#include <QRegExpValidator>
#include <QCryptographicHash>
#include <QMessageBox>

LoginDialog::LoginDialog(Rpc::StartPrx startPrx, QWidget* parent) : QDialog(parent), startPrx_(startPrx)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.loginDialogTab->tabBar()->setObjectName("loginDialogTabBar");

	QRegExp usernameExp(R"([A-Za-z][_A-Za-z0-9]*)");

	ui_.loginUsernameEdit->setValidator(new QRegExpValidator(usernameExp));
	ui_.signupUsernameEdit->setValidator(new QRegExpValidator(usernameExp));

	QObject::connect(ui_.loginButton, &QPushButton::clicked, this, &LoginDialog::login);
	QObject::connect(ui_.signupButton, &QPushButton::clicked, this, &LoginDialog::signup);
}

LoginDialog::~LoginDialog()
{
}

Rpc::SessionPrx LoginDialog::session()
{
	return sessionPrx_;
}

void LoginDialog::login()
{
	QString username = ui_.loginUsernameEdit->text();
	QString password = ui_.loginPasswordEdit->text();

	password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toBase64();

	Rpc::ErrorCode ec = startPrx_->login(username.toStdString(), password.toStdString(), sessionPrx_);

	if (ec == Rpc::ec_success) {
		done(1);
	}
	else {
		QMessageBox msg;
		msg.setWindowTitle("Base");
		msg.setText(errorMessage(ec));
		msg.exec();
	}
}

void LoginDialog::signup()
{
	QString username = ui_.signupUsernameEdit->text();
	QString password = ui_.signupPasswordEdit->text();
	QString password2 = ui_.signupPasswordEdit2->text();

	if (username.size() < 4) {
		QMessageBox::information(this, "Base", "Username is too short.");
		return;
	}

	if (password.size() < 6) {
		QMessageBox::information(this, "Base", "Password is too short.");
		return;
	}

	if (password2 != password) {
		QMessageBox::information(this, "Base", "These passwords don't match.");
		return;
	}

	password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toBase64();

	Rpc::ErrorCode ec = startPrx_->signup(username.toStdString(), password.toStdString());

	QMessageBox msg;
	msg.setWindowTitle("Base");
	msg.setText(errorMessage(ec));
	msg.exec();

	if (ec == Rpc::ec_success) {
		ui_.signupUsernameEdit->clear();
		ui_.signupPasswordEdit->clear();
		ui_.signupPasswordEdit2->clear();
		ui_.signupUsernameEdit->setFocus();
		ui_.loginDialogTab->setCurrentIndex(0);
		ui_.loginUsernameEdit->setFocus();
	}
}

