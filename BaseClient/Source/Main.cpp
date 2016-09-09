#include "LoginDialog.h"
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

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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

	try {
		ic = Ice::initialize(Ice::StringSeq{"--Ice.Config=BaseClient.cfg"});

		Rpc::StartPrx startPrx = Rpc::StartPrx::checkedCast(ic->propertyToProxy("Start"));

		std::string serverVersion = startPrx->getServerVersion();
		if (serverVersion != "1.0.0.2") {
			QMessageBox::information(0, "Base", QString("Unmatched server version (%1).").arg(serverVersion.c_str()));
			return 0;
		}

		LoginDialog ld(startPrx);
		const int rc = ld.exec();

		if (rc != 1) {
			return 0;
		}

		BaseClient w(ld.session());

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

	if (ic) {
		ic->destroy();
	}

	return returnCode;
}

