#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>

module Rpc
{
	struct EngineVersionItem
	{
		string name;
		string version;
		string uptime;
		string info;
		string state;
	};

	sequence<EngineVersionItem> EngineVersionItemSeq;

	interface EngineVersionBrowser
	{
		ErrorCode next(int n, out EngineVersionItemSeq items);
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

		ErrorCode browseEngineVersions(out EngineVersionBrowser* browser);
		ErrorCode uploadEngineVersion(string name, string version, string info, out Uploader* uploader);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
	};
};

