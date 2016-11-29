#pragma once

#include <RpcErrorCode.ice>
#include <RpcSession.ice>
#include <RpcDownloader.ice>

module Rpc
{
	interface Start
	{
		string getServerVersion();
		string getClientVersion();
		ErrorCode downloadClient(out Downloader* downloader);
		ErrorCode login(string username, string password, out Session* session);
	};
};
