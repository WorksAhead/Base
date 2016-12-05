#include "LoginDialog.h"
#include "DownloadClientDialog.h"
#include "UpdateDialog.h"
#include "BaseClient.h"
#include "QtUtils.h"

#include "RpcStart.h"

#include <QApplication>
#include <QSharedMemory>
#include <QStyleFactory>
#include <QFile>
#include <QPalette>
#include <QFont>
#include <QMessageBox>
#include <QLocalSocket>
#include <QSettings>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <fstream>
#include <vector>
#include <string>

#if WIN32
#define NOMINMAX
#include <windows.h>
#endif

#define BASE_CURRENT_VERSION "1.0.0.20"

namespace fs = boost::filesystem;

bool versionLess(const std::string& lhs, const std::string& rhs)
{
	std::vector<std::string> v1;
	boost::split(v1, lhs, boost::is_any_of("."));

	std::vector<std::string> v2;
	boost::split(v2, rhs, boost::is_any_of("."));

	for (int i = 0; i < 4; ++i)
	{
		int a = std::stoi(v1.at(i));
		int b = std::stoi(v2.at(i));

		if (a < b) {
			return true;
		}
		else if (a > b) {
			return false;
		}
	}

	return false;
}

int main(int argc, char* argv[])
{
	fs::path workDir = fs::canonical(fs::path(argv[0]).parent_path());

	QApplication app(argc, argv);

	app.setStyle(QStyleFactory::create("Fusion"));

	QPalette pal = app.palette();
	pal.setColor(QPalette::Link, QColor(180, 180, 180));
	pal.setColor(QPalette::LinkVisited, QColor(128, 128, 128));
	pal.setColor(QPalette::Window, QColor(40, 40, 40));
	pal.setColor(QPalette::Background, QColor(40, 40, 40));
	pal.setColor(QPalette::Base, QColor(30, 30, 30));
	pal.setColor(QPalette::WindowText, QColor(250, 250, 250));
	pal.setColor(QPalette::Text, QColor(250, 250, 250));
	app.setPalette(pal);

	QFile qss(":/BaseClient.qss");
	if (qss.open(QIODevice::ReadOnly)) {
		app.setStyleSheet(QString(qss.readAll()));
	}

	QFont font;
	//font.setFamily(QStringLiteral("Segoe UI"));
	font.setFamily(QStringLiteral("Microsoft YaHei UI"));
	//font.setPointSize(9);
	app.setFont(font);

	QSharedMemory shm(&app);
	shm.setKey("base_client_application_first_instance");
	bool isFirstInstance = shm.create(1024, QSharedMemory::ReadWrite);

	std::vector<std::string> arguments;

	for (int i = 1; i < argc; ++i) {
		arguments.push_back(argv[i]);
	}

	std::string url;

	bool dev = false;

	while (!arguments.empty())
	{
		if (arguments[0] == "ver") {
			std::fstream os((workDir.parent_path() / "Version").string().c_str(), std::ios::out);
			os << BASE_CURRENT_VERSION << "\n";
			os.flush();
			return 0;
		}
		else if (arguments[0] == "url" && arguments.size() >= 2) {
			url = arguments[1];
			if (!isFirstInstance) {
				QLocalSocket socket(&app);
				socket.connectToServer("base_client_application_uri_server", QIODevice::WriteOnly);
				if (socket.waitForConnected(1000)) {
					QByteArray block;
					QDataStream out(&block, QIODevice::WriteOnly);
					out.setVersion(QDataStream::Qt_5_5);
					out << QString(url.c_str());
					socket.write(block);
					socket.waitForBytesWritten();
					socket.disconnectFromServer();
				}
				return 0;
			}
			arguments.erase(arguments.begin(), arguments.begin() + 2);
		}
		else if (arguments[0] == "setup") {
#if WIN32
			QSettings s1("HKEY_CLASSES_ROOT\\base", QSettings::NativeFormat);
			s1.setValue("Default", "URL:Base Protocol");
			s1.setValue("URL Protocol", "");
			QSettings s2("HKEY_CLASSES_ROOT\\base\\shell\\open\\command", QSettings::NativeFormat);
			std::string program = (workDir / "BaseClient.exe").string();
			boost::replace_all(program, "/", "\\");
			std::string cmd = "\"" + program + "\" url \"%1\"";
			s2.setValue("Default", QString::fromLocal8Bit(cmd.c_str()));
#endif
			return 0;
		}
		else if (arguments[0] == "update" && arguments.size() >= 3) {
			UpdateDialog d(QString::fromLocal8Bit(arguments[1].c_str()), QString::fromLocal8Bit(arguments[2].c_str()));
			d.exec();
			return 0;
		}
		else if (arguments[0] == "dev") {
			dev = true;
			arguments.erase(arguments.begin(), arguments.begin() + 1);
		}
		else {
			QMessageBox::information(0, "Base", "Bad command line syntax.");
			return 0;
		}
	}

	if (!isFirstInstance) {
		return 0;
	}

	if (!fs::exists(workDir / "IgnorePath") && workDir.string().size() > 24)
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle("Base");
		msgBox.setText("Because some Engines/SDKs/Tools do not support extended-length path (beyond 260 characters), "
			"please put BaseClient in a shorter path (within 24 characters) location to avoid possible errors.");
		QPushButton* closeButton = msgBox.addButton("Close", QMessageBox::NoRole);
		QPushButton* ignoreButton = msgBox.addButton("Ignore", QMessageBox::NoRole);
		msgBox.setDefaultButton(closeButton);
		msgBox.exec();

		if (msgBox.clickedButton() == ignoreButton) {
			std::fstream f((workDir / "IgnorePath").string().c_str(), std::ios::out);
		}
		else {
			return 0;
		}
	}

#if WIN32
	if (!fs::exists(workDir / "IgnoreProtocol"))
	{
		bool notRegistered = false;

		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Question);
		QPushButton* yesButton = msgBox.addButton("Yes", QMessageBox::NoRole);
		QPushButton* ignoreButton = msgBox.addButton("Ignore", QMessageBox::NoRole);
		msgBox.setDefaultButton(yesButton);

		QString cmd = QSettings("HKEY_CLASSES_ROOT\\base\\shell\\open\\command", QSettings::NativeFormat).value("Default", "").toString();
		QStringList args = parseCombinedArgString(cmd);
		if (args.isEmpty()) {
			msgBox.setText("There is no default Base Client to handle Base URL Protocol, do you want to make current Base Client the default ?\n"
				"(This operation requires administrative privileges)");
			notRegistered = true;
		}
		else if (!fs::equivalent(fs::path(toLocal8bit(args.front())).parent_path(), workDir)) {
			msgBox.setText("Current Base Client is not the default Client to handle Base URL Protocol, do you want to make current Base Client the default ?\n"
				"(This operation requires administrative privileges)");
			notRegistered = true;
		}

		if (notRegistered)
		{
			msgBox.exec();

			if (msgBox.clickedButton() == yesButton)
			{
				std::string cmd = (workDir / "BaseClient.exe").string();
				SHELLEXECUTEINFOA shExInfo;
				shExInfo.cbSize = sizeof(shExInfo);
				shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				shExInfo.hwnd = 0;
				shExInfo.lpVerb = "runas";
				shExInfo.lpFile = cmd.c_str();
				shExInfo.lpParameters = "setup";
				shExInfo.lpDirectory = 0;
				shExInfo.nShow = SW_SHOW;
				shExInfo.hInstApp = 0;
				if (ShellExecuteExA(&shExInfo)) {
					WaitForSingleObject(shExInfo.hProcess, INFINITE);
					CloseHandle(shExInfo.hProcess);
				}
			}
			else if (msgBox.clickedButton() == ignoreButton) {
				std::fstream f((workDir / "IgnoreProtocol").string().c_str(), std::ios::out);
			}
		}
	}
#endif // WIN32

	int returnCode = 1;

	Ice::CommunicatorPtr ic;

	BOOST_SCOPE_EXIT_ALL(&ic)
	{
		if (ic) {
			ic->destroy();
		}
	};

	try {
		ic = Ice::initialize(Ice::StringSeq{"--Ice.Config=" + fromLocal8bit((workDir / "BaseClient.cfg").string())});

		Rpc::StartPrx startPrx = Rpc::StartPrx::checkedCast(ic->propertyToProxy("Start"));

		std::string clientVersion = startPrx->getClientVersion();

		if (clientVersion.empty())
		{
			QMessageBox::warning(0, "Base", "No active Client version.");
		}
		else if (BASE_CURRENT_VERSION != clientVersion)
		{
			if (versionLess(BASE_CURRENT_VERSION, clientVersion)) {
				DownloadClientDialog d(startPrx);
				d.exec();
				return 0;
			}
			else if (!dev) {
				QMessageBox::information(0, "Base",
					QString("Current version (%1) is newer than required version (%2).").arg(BASE_CURRENT_VERSION).arg(clientVersion.c_str()));
				return 0;
			}
		}

		Ice::PropertiesPtr props = ic->getProperties();

		app.setProperty("BaseClient.HttpUrlRedir", QString(props->getProperty("BaseClient.HttpUrlRedir").c_str()));

		LoginDialog ld(QString::fromLocal8Bit(workDir.string().c_str()), startPrx);
		const int rc = ld.exec();

		if (rc != 1) {
			return 0;
		}

		BaseClient w(QString::fromLocal8Bit(workDir.parent_path().string().c_str()), BASE_CURRENT_VERSION, ld.session(), url.c_str());

		w.setMinimumSize(800, 500);
		w.resize(1280, 800);
		w.show();

		returnCode = app.exec();
	}
	catch (const Ice::Exception& e) {
		QMessageBox::information(0, "Base", e.what());
	}
	catch (std::exception& e) {
		QMessageBox::information(0, "Base", e.what());
	}
	catch (const char* msg) {
		//std::cout << msg << std::endl;
	}
	catch (...) {
	}

	return returnCode;
}

