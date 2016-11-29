#include "LoginDialog.h"
#include "QtUtils.h"

#include <ErrorMessage.h>

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QByteArray>

#include <boost/filesystem.hpp>

#include "Security/Md5.h"
#include "Security/Base64.h"

namespace fs = boost::filesystem;

LoginDialog::LoginDialog(const QString& workPath, Rpc::StartPrx startPrx, QWidget* parent) : QDialog(parent), startPrx_(startPrx)
{
	workPath_ = toLocal8bit(workPath);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui_.setupUi(this);

	ui_.loginDialogTab->tabBar()->setObjectName("loginDialogTabBar");

	QObject::connect(ui_.rememberPasswordCheckBox, &QCheckBox::clicked, [this](bool checked){
		if (checked) {
			ui_.rememberUsernameCheckBox->setChecked(true);
		}
	});

	QObject::connect(ui_.loginButton, &QPushButton::clicked, this, &LoginDialog::onLogin);

	QFile file(QString::fromLocal8Bit((fs::path(workPath_) / "RememberMe").string().c_str()));

	if (file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
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

	char* key = "1qaz2wsx3edc4rfv";
	rijndael_.MakeKey(key, CRijndael::sm_chain0, 16, 16);
}

LoginDialog::~LoginDialog()
{
}

Rpc::SessionPrx LoginDialog::session()
{
	return sessionPrx_;
}

QByteArray LoginDialog::encrypt(const QString& s)
{
	QByteArray in(s.toUtf8());

	const int size = in.size();
	const int alignedSize = (size + 16 - 1) & ~(16 - 1);

	in.resize(alignedSize);

	memset(in.data() + size, 0, alignedSize - size);

	QByteArray out(alignedSize, Qt::Uninitialized);

	rijndael_.Encrypt(in.data(), out.data(), alignedSize);

	return out;
}

void LoginDialog::onLogin()
{
	QString username = ui_.loginUsernameEdit->text();
	QString password = ui_.loginPasswordEdit->text();

	QString encodedPassword;

	if (password == "\r\r\r\r") {
		encodedPassword = password_;
	}
	else {
		encodedPassword = encrypt(password).toBase64();
	}

	Rpc::ErrorCode ec = startPrx_->login(username.toStdString(), encodedPassword.toStdString(), sessionPrx_);

	if (ec == Rpc::ec_success)
	{
		if (ui_.rememberUsernameCheckBox->isChecked()) {
			QFile file(QString::fromLocal8Bit((fs::path(workPath_) / "RememberMe").string().c_str()));
			if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
				QTextStream out(&file);
				out << username << "\n";
				if (ui_.rememberPasswordCheckBox->isChecked()) {
					out << encodedPassword << "\n";
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
