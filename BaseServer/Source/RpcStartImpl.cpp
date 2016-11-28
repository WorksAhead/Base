#include "RpcStartImpl.h"
#include "RpcSessionImpl.h"
#include "RpcClientDownloaderImpl.h"
#include "Datetime.h"
#include "Context.h"

#include <Ice/Ice.h>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <boost/filesystem.hpp>

#include <sstream>

namespace fs = boost::filesystem;

RpcStartImpl::RpcStartImpl(CenterPtr center) : center_(center)
{
	timer_ = new IceUtil::Timer();
	maintainer_ = new RpcSessionMaintainer;
	timer_->scheduleRepeated(maintainer_, IceUtil::Time::seconds(1));
}

std::string RpcStartImpl::getServerVersion(const Ice::Current&)
{
	return "1.0.0.4";
}

std::string RpcStartImpl::getClientVersion(const Ice::Current&)
{
	return center_->getNewestClientVersion();
}

Rpc::ErrorCode RpcStartImpl::downloadClient(Rpc::DownloaderPrx& downloaderPrx, const Ice::Current& c)
{
	RpcClientDownloaderImplPtr downloader = new RpcClientDownloaderImpl(center_);

	downloader->init(center_->getNewestClientVersion());

	downloaderPrx = Rpc::DownloaderPrx::uncheckedCast(c.adapter->addWithUUID(downloader));

	maintainer_->add(std::make_pair(downloaderPrx, downloader));

	return Rpc::ec_success;
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

	SQLite::Transaction t(*center_->db());
	const int n = center_->db()->exec(oss.str());
	t.commit();

	return (n == 1) ? Rpc::ec_success : Rpc::ec_username_already_exists;
}

Rpc::ErrorCode RpcStartImpl::login(const std::string& username, const std::string& password, Rpc::SessionPrx& sessionPrx, const Ice::Current& c)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Users";
	oss << " WHERE ";
	oss << "Username=" << sqlText(username);
	oss << " AND ";
	oss << "Password=" << sqlText(password);

	SQLite::Statement s(*center_->db(), oss.str());
	s.executeStep();
	if (!s.isOk()) {
		return Rpc::ec_username_or_password_incorrect;
	}

	ContextPtr context(new Context(center_));

	context->setUser(s.getColumn("Username").getText());
	context->setUserGroup(s.getColumn("Group").getText());

	RpcSessionImplPtr session = new RpcSessionImpl(context);
	sessionPrx = Rpc::SessionPrx::uncheckedCast(c.adapter->addWithUUID(session));

	maintainer_->add(std::make_pair(sessionPrx, session));

	return Rpc::ec_success;
}

Rpc::ErrorCode RpcStartImpl::resetPassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword, const Ice::Current&)
{
	if (center_->resetUserPassword(username, oldPassword, newPassword)) {
		return Rpc::ec_success;
	}
	else {
		return Rpc::ec_username_or_password_incorrect;
	}
}

