#pragma once

#include <RpcErrorCode.ice>
#include <RpcSession.ice>

module Rpc
{
	interface Login
	{
		ErrorCode signup(string name, string password);
		Session* login(string name, string password);
	};
};
