#include "Script.h"

#include <QFileInfo>

Script::Script()
{
	state_["fs"] = state_.newTable();

	state_["fs"]["exists"] = kaguya::function([](const std::string& path)->bool
	{
		QFileInfo info(path.c_str());
		return info.exists();
	});

	state_["fs"]["isDirectory"] = kaguya::function([](const std::string& path)->bool
	{
		QFileInfo info(path.c_str());
		return info.isDir();
	});

	state_["fs"]["isFile"] = kaguya::function([](const std::string& path)->bool
	{
		QFileInfo info(path.c_str());
		return info.isFile();
	});

	state_.setErrorHandler([](int, const char* message) {
		throw std::runtime_error(message);
	});
}

Script::~Script()
{

}

bool Script::loadFromString(const std::string& code)
{
	return state_.dostring(code);
}

bool Script::loadFromServer(ContextPtr context, const char* uniformInfo)
{
	std::string code;

	Rpc::ErrorCode ec = context->session->getUniformInfo(uniformInfo, code);

	if (ec != Rpc::ec_success)
	{
		context->promptRpcError(ec);
		return false;
	}

	return state_.dostring(code);
}

