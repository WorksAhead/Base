#include "LoginDialog.h"
#include "DownloadClientDialog.h"
#include "UpdateDialog.h"
#include "BaseClient.h"
#include "QtUtils.h"

#include "RpcStart.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QPalette>
#include <QFont>
#include <QMessageBox>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <fstream>
#include <vector>
#include <string>

#define BASE_CURRENT_VERSION "1.0.0.11"

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
	QApplication app(argc, argv);

	app.setStyle(QStyleFactory::create("Fusion"));

	QPalette pal = app.palette();
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

	std::vector<std::string> arguments;

	for (int i = 1; i < argc; ++i) {
		arguments.push_back(argv[i]);
	}

	bool dev = false;

	if (!arguments.empty())
	{
		if (arguments[0] == "ver") {
			std::fstream os((fs::current_path().parent_path() / "version").string().c_str(), std::ios::out);
			os << BASE_CURRENT_VERSION << "\n";
			os.flush();
			return 0;
		}
		else if (arguments[0] == "update" && arguments.size() == 3) {
			UpdateDialog d(QString::fromLocal8Bit(arguments[1].c_str()), QString::fromLocal8Bit(arguments[2].c_str()));
			d.exec();
			return 0;
		}
		else if (arguments[0] == "dev") {
			dev = true;
		}
		else {
			QMessageBox::information(0, "Base", "Bad command line syntax.");
			return 0;
		}
	}

	if (fs::canonical(fs::current_path()).parent_path().string().size() > 24)
	{
		QFile file("IgnorePath");
		if (!file.open(QIODevice::ReadOnly)) {
			int ret = QMessageBox::warning(
				0,
				"Base",
				"Because some Engines/SDKs/Tools do not support extended-length path (beyond 260 characters), "
				"please put BaseClient in a shorter path (within 24 characters) location to avoid possible errors.",
				QMessageBox::Ignore,
				QMessageBox::Close|QMessageBox::Default);

			if (ret == QMessageBox::Close) {
				return 0;
			}
			else if (ret == QMessageBox::Ignore) {
				QFile file("IgnorePath");
				if (file.open(QIODevice::WriteOnly)) {
					file.close();
				}
			}
		}
	}

	int returnCode = 1;

	Ice::CommunicatorPtr ic;

	BOOST_SCOPE_EXIT_ALL(&ic)
	{
		if (ic) {
			ic->destroy();
		}
	};

	try {
		ic = Ice::initialize(Ice::StringSeq{"--Ice.Config=BaseClient.cfg"});

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

		LoginDialog ld(startPrx);
		const int rc = ld.exec();

		if (rc != 1) {
			return 0;
		}

		BaseClient w(BASE_CURRENT_VERSION, ld.session());

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

