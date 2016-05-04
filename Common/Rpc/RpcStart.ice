#pragma once

#include <RpcErrorCode.ice>
#include <RpcSession.ice>

module Rpc
{
	interface Start
	{
		string getServerVersion();
		ErrorCode signup(string username, string password);
		ErrorCode login(string username, string password, out Session* session);
	};
};
