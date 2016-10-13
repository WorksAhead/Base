#include "LoginDialog.h"
#include "QtUtils.h"

#include <ErrorMessage.h>

#include <QRegExpValidator>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

LoginDialog::LoginDialog(const QString& workPath, Rpc::StartPrx startPrx, QWidget* parent) : QDialog(parent), startPrx_(startPrx)
{
	workPath_ = toLocal8bit(workPath);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.loginDialogTab->tabBar()->setObjectName("loginDialogTabBar");

	QRegExp usernameExp(R"([A-Za-z][_A-Za-z0-9]*)");

	ui_.loginUsernameEdit->setValidator(new QRegExpValidator(usernameExp));
	ui_.signupUsernameEdit->setValidator(new QRegExpValidator(usernameExp));
	ui_.resetUsernameEdit->setValidator(new QRegExpValidator(usernameExp));

	QObject::connect(ui_.rememberPasswordCheckBox, &QCheckBox::clicked, [this](bool checked){
		if (checked) {
			ui_.rememberUsernameCheckBox->setChecked(true);
		}
	});

	QObject::connect(ui_.loginButton, &QPushButton::clicked, this, &LoginDialog::onLogin);
	QObject::connect(ui_.signupButton, &QPushButton::clicked, this, &LoginDialog::onSignup);
	QObject::connect(ui_.resetButton, &QPushButton::clicked, this, &LoginDialog::onReset);

	QFile file(QString::fromLocal8Bit((fs::path(workPath_) / "RememberMe").string().c_str()));
	if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		QTextStream in(&file);
		QString username = in.readLine();
		if (!username.isNull()) {
			ui_.loginUsernameEdit->setText(username);
			ui_.rememberUsernameCheckBox->setChecked(true);
			QString password = in.readLine();
			if (!password.isNull()) {
				password_ = password;
				ui_.loginPasswordEdit->setText("\r\r\r\r");
				ui_.rememberPasswordCheckBox->setChecked(true);
				ui_.loginButton->setFocus();
			}
			else {
				ui_.loginPasswordEdit->setFocus();
			}
		}
	}
}

LoginDialog::~LoginDialog()
{
}

Rpc::SessionPrx LoginDialog::session()
{
	return sessionPrx_;
}

void LoginDialog::onLogin()
{
	QString username = ui_.loginUsernameEdit->text();
	QString password = ui_.loginPasswordEdit->text();

	if (password == "\r\r\r\r") {
		password = password_;
	}
	else {
		password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toBase64();
	}

	Rpc::ErrorCode ec = startPrx_->login(username.toStdString(), password.toStdString(), sessionPrx_);

	if (ec == Rpc::ec_success)
	{
		if (ui_.rememberUsernameCheckBox->isChecked()) {
			QFile file(QString::fromLocal8Bit((fs::path(workPath_) / "RememberMe").string().c_str()));
			if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
				QTextStream out(&file);
				out << username << "\n";
				if (ui_.rememberPasswordCheckBox->isChecked()) {
					out << password << "\n";
				}
			}
		}
		else {
			QFile file(QString::fromLocal8Bit((fs::path(workPath_) / "RememberMe").string().c_str()));
			file.remove();
		}
		done(1);
	}
	else {
		QMessageBox::information(this, "Base", errorMessage(ec));
	}
}

void LoginDialog::onSignup()
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

	QMessageBox::information(this, "Base", errorMessage(ec));

	if (ec == Rpc::ec_success) {
		ui_.signupUsernameEdit->clear();
		ui_.signupPasswordEdit->clear();
		ui_.signupPasswordEdit2->clear();
		ui_.signupUsernameEdit->setFocus();
		ui_.loginDialogTab->setCurrentIndex(0);
		ui_.loginUsernameEdit->setFocus();
	}
}

void LoginDialog::onReset()
{
	QString username = ui_.resetUsernameEdit->text();
	QString password = ui_.resetPasswordEdit->text();
	QString newPassword = ui_.resetNewPasswordEdit->text();
	QString newPassword2 = ui_.resetNewPasswordEdit2->text();

	if (newPassword.size() < 6) {
		QMessageBox::information(this, "Base", "Password is too short.");
		return;
	}

	if (newPassword2 != newPassword) {
		QMessageBox::information(this, "Base", "These passwords don't match.");
		return;
	}

	password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toBase64();
	newPassword = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Md5).toBase64();

	Rpc::ErrorCode ec = startPrx_->resetPassword(username.toStdString(), password.toStdString(), newPassword.toStdString());

	QMessageBox::information(this, "Base", errorMessage(ec));

	if (ec == Rpc::ec_success) {
		ui_.resetUsernameEdit->clear();
		ui_.resetPasswordEdit->clear();
		ui_.resetNewPasswordEdit->clear();
		ui_.resetNewPasswordEdit2->clear();
		ui_.resetUsernameEdit->setFocus();
		ui_.loginDialogTab->setCurrentIndex(0);
		ui_.loginUsernameEdit->setFocus();
	}
}

