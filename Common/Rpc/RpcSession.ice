#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>

module Rpc
{
	interface ContentBrowser
	{
		void finish();
	};

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
		void finish();
	};

	interface Uploader
	{
		ErrorCode write(long offset, ["cpp:array"] ByteSeq bytes);
		ErrorCode finish(int crc32);
		void cancel();
	};

	interface Downloader
	{
		ErrorCode getSize(out long size);
		ErrorCode read(long offset, int size, out ByteSeq bytes);
		void finish();
	};

	interface Session
	{
		void destroy();
		void refresh();

		ErrorCode setPages(StringSeq pages);
		ErrorCode getPages(out StringSeq pages);

		ErrorCode setCategories(StringSeq categories);
		ErrorCode getCategories(out StringSeq categories);

		ErrorCode browseContent(string page, string category, string orderBy, out ContentBrowser* browser);

		ErrorCode browseEngineVersions(out EngineVersionBrowser* browser);
		ErrorCode uploadEngineVersion(string name, string version, string info, out Uploader* uploader);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
	};
};

