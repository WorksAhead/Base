#include "RpcStartImpl.h"
#include "RpcSessionImpl.h"
#include "Datetime.h"

#include <Ice/Ice.h>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <sstream>

RpcStartImpl::RpcStartImpl(RpcSessionMaintainerPtr maintainer, DatabasePtr db) : maintainer_(maintainer), db_(db)
{
}

std::string RpcStartImpl::getServerVersion(const Ice::Current&)
{
	return "1.0.0.0";
}

Rpc::ErrorCode RpcStartImpl::signup(const std::string& username, const std::string& password, const Ice::Current&)
{
	std::ostringstream oss;
	oss << "INSERT OR IGNORE INTO Users VALUES (";
	oss << sqlText(username) << ", ";
	oss << sqlText(password) << ", ";
	oss << sqlText("User") << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText("") << ")";

	SQLite::Transaction t(*db_);
	const int n = db_->exec(oss.str());
	t.commit();

	return (n == 1) ? Rpc::ec_success : Rpc::username_already_exists;
}

Rpc::ErrorCode RpcStartImpl::login(const std::string& username, const std::string& password, Rpc::SessionPrx& sessionPrx, const Ice::Current& c)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Users";
	oss << " WHERE ";
	oss << "Username=" << sqlText(username);
	oss << " AND ";
	oss << "Password=" << sqlText(password);

	SQLite::Statement s(*db_, oss.str());
	s.executeStep();
	if (!s.isOk()) {
		return Rpc::username_or_password_incorrect;
	}

	RpcSessionImplPtr session = new RpcSessionImpl(db_);
	sessionPrx = Rpc::SessionPrx::uncheckedCast(c.adapter->addWithUUID(session));

	maintainer_->add(std::make_pair(sessionPrx, session));

	return Rpc::ec_success;
}

