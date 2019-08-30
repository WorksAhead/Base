#include "RpcStartImpl.h"
#include "RpcSessionImpl.h"
#include "RpcClientDownloaderImpl.h"
#include "Datetime.h"
#include "Context.h"

#include "Security/Base64.h"

#include <Ice/Ice.h>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <boost/filesystem.hpp>

#include <sstream>

#include <curl/curl.h>

namespace fs = boost::filesystem;

RpcStartImpl::RpcStartImpl(CenterPtr center) : center_(center)
{
	timer_ = new IceUtil::Timer();
	maintainer_ = new RpcSessionMaintainer;
	timer_->scheduleRepeated(maintainer_, IceUtil::Time::seconds(1));

	const char* key = "1qaz2wsx3edc4rfv";
	rijndael_.MakeKey(key, CRijndael::sm_chain0, 16, 16);
}

std::string RpcStartImpl::getServerVersion(const Ice::Current&)
{
	return "1.0.0.16";
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

Rpc::ErrorCode RpcStartImpl::login(const std::string& username, const std::string& password, Rpc::SessionPrx& sessionPrx, const Ice::Current& c)
{
	std::string blockPassword = base64_decode(password);

	if ((blockPassword.size() & (16 - 1)) != 0) {
		return Rpc::ec_username_or_password_incorrect;
	}

	std::string cleartextPassword(blockPassword.size(), '\0');

	rijndael_.Decrypt(&blockPassword[0], &cleartextPassword[0], blockPassword.size());

	if (loginToCYou(username, cleartextPassword))
	{
		std::ostringstream oss;
		oss << "INSERT OR IGNORE INTO Users VALUES (";
		oss << sqlText(username) << ", ";
		oss << sqlText("User") << ", ";
		oss << sqlText("") << ")";

		SQLite::Transaction t(*center_->db());
		center_->db()->exec(oss.str());
		t.commit();
	}
	else {
		return Rpc::ec_username_or_password_incorrect;
	}

	std::ostringstream oss;
	oss << "SELECT * FROM Users";
	oss << " WHERE ";
	oss << "Username=" << sqlText(username);

	SQLite::Statement s(*center_->db(), oss.str());
	s.executeStep();
	if (!s.isOk()) {
		return Rpc::ec_username_does_not_exist;
	}

	ContextPtr context(new Context(center_));

	context->setUser(s.getColumn("Username").getText());
	context->setUserGroup(s.getColumn("Group").getText());

	RpcSessionImplPtr session = new RpcSessionImpl(context);
	sessionPrx = Rpc::SessionPrx::uncheckedCast(c.adapter->addWithUUID(session));

	maintainer_->add(std::make_pair(sessionPrx, session));

	return Rpc::ec_success;
}

bool RpcStartImpl::loginToCYou(const std::string& username, const std::string& clear_pwd)
{
	return true;

	CURLcode res = CURL_LAST;

	std::string user_pwd = username + ":" + clear_pwd;
	static const char *ldap_url = "LDAP://10.1.0.12/OU=Users,OU=Managed,DC=cyou-inc,DC=com";

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

