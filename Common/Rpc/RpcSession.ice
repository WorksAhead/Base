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

	interface Uploader
	{
		ErrorCode write(long offset, ["cpp:array"] ByteSeq bytes);
		ErrorCode finish(int crc32);
		void cancel();
	};

	interface Downloader
	{
		ErrorCode read(long offset, int num, out ByteSeq bytes);
		ErrorCode finish();
		void cancel();
	};

	interface Session
	{
		void destroy();
		void refresh();

		ErrorCode browseEngines(out EngineBrowser* browser);
		ErrorCode uploadEngine(string name, string version, string info, out Uploader* uploader);
		ErrorCode downloadEngine(string name, string version, out Downloader* downloader);
		ErrorCode removeEngine(string name, string version);
	};
};

