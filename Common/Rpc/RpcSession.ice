#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>

module Rpc
{
	struct EngineItem
	{
		string name;
		string version;
		string uptime;
		string info;
		string state;
	};

	sequence<EngineItem> EngineItemSeq;

	interface EngineBrowser
	{
		ErrorCode next(int n, out EngineItemSeq items);
		ErrorCode finish();
	};

	interface EngineUploader
	{
		ErrorCode write(long offset, ["cpp:array"] ByteSeq bytes);
		ErrorCode finish(int crc32);
	};

	interface EngineDownloader
	{
		ErrorCode read(long offset, int num, out ByteSeq bytes);
		ErrorCode finish();
	};

	interface Session
	{
		void destroy();
		void refresh();

		ErrorCode browseEngines(out EngineBrowser* browser);
		ErrorCode uploadEngine(string name, string version, string info, out EngineUploader* uploader);
		ErrorCode downloadEngine(string name, string version, out EngineDownloader* downloader);
		ErrorCode removeEngine(string name, string version);
	};
};

