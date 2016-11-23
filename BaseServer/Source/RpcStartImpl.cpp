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

#include <curl/curl.h>

#include "Security/Base64.h"
#include "Security/Md5.h"

namespace fs = boost::filesystem;

RpcStartImpl::RpcStartImpl(CenterPtr center) : center_(center)
{
	timer_ = new IceUtil::Timer();
	maintainer_ = new RpcSessionMaintainer;
	timer_->scheduleRepeated(maintainer_, IceUtil::Time::seconds(1));

	char* key = "1qaz2wsx3edc4rfv";
	rijndael_.MakeKey(key, CRijndael::sm_chain0, 16, 16);
}

std::string RpcStartImpl::getServerVersion(const Ice::Current&)
{
	return "1.0.0.3";
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
	std::string clear_pwd = decrypt(password);
	clear_pwd = base64_encode(reinterpret_cast<const unsigned char*>(md5(clear_pwd).c_str()), (unsigned int)clear_pwd.length());

	std::ostringstream oss;
	oss << "INSERT OR IGNORE INTO Users VALUES (";
	oss << sqlText(username) << ", ";
	oss << sqlText(clear_pwd) << ", ";
	oss << sqlText("User") << ", ";
	oss << sqlText(getCurrentTimeString()) << ", ";
	oss << sqlText("") << ")";

	SQLite::Transaction t(*center_->db());
	const int n = center_->db()->exec(oss.str());
	t.commit();

	return (n == 1) ? Rpc::ec_success : Rpc::ec_username_already_exists;
}

std::string RpcStartImpl::decrypt(const string& pwd)
{
	char decrypted[16];
	rijndael_.Decrypt(pwd.c_str(), decrypted, 16);

	return decrypted;
}

bool RpcStartImpl::loginToCYou(const std::string& username, const std::string& clear_pwd)
{
	std::string cyou_username = username + "@cyou-inc.com";

	CURLcode res = CURL_LAST;
	
	std::string user_pwd = cyou_username + ":" + clear_pwd;
	static char *ldap_url = "LDAP://10.1.0.11/OU=Users,OU=Managed,DC=cyou-inc,DC=com";

	curl_global_init(CURL_GLOBAL_ALL);

	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, ldap_url);
		curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd.c_str());
		
		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return res == CURLE_OK;
}

Rpc::ErrorCode RpcStartImpl::login(const std::string& username, const std::string& password, Rpc::SessionPrx& sessionPrx, const Ice::Current& c)
{
	std::string clear_pwd = decrypt(password);

	/*
	if (!loginToCYou(username, clear_pwd))
	{
		return Rpc::ec_username_or_password_incorrect;
	}
	*/

	clear_pwd = base64_encode(reinterpret_cast<const unsigned char*>(md5(clear_pwd).c_str()), (unsigned int)clear_pwd.length());

	std::ostringstream oss;
	oss << "SELECT * FROM Users";
	oss << " WHERE ";
	oss << "Username=" << sqlText(username);
	oss << " AND ";
	oss << "Password=" << sqlText(clear_pwd);


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

