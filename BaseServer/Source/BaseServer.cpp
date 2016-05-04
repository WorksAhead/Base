#include "BaseServer.h"
#include "RpcStartImpl.h"

#include "RpcSessionMaintainer.h"

#include <SQLiteCpp/Database.h>

bool BaseServer::start(int argc, char* argv[], int& status)
{
	try {
		timer_ = new IceUtil::Timer();
		RpcSessionMaintainerPtr maintainer = new RpcSessionMaintainer;
		timer_->scheduleRepeated(maintainer, IceUtil::Time::seconds(1));

		DatabasePtr db(new SQLite::Database("BaseServer.db", SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE));
		db->exec("CREATE TABLE IF NOT EXISTS Users ("
			"Username TEXT COLLATE NOCASE UNIQUE, Password TEXT, \"Group\" TEXT, "
			"RegTime DATETIME, Info TEXT)");

		adapter_ = communicator()->createObjectAdapter("BaseServer");
		adapter_->add(new RpcStartImpl(maintainer, db), communicator()->stringToIdentity("Start"));
		adapter_->activate();
	}
	catch (Ice::LocalException&) {
		status = EXIT_FAILURE;
		throw;
	}

	status = EXIT_SUCCESS;
	return true;
}

bool BaseServer::stop()
{
	return true;
}

