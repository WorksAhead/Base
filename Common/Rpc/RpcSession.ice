#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>
#include <RpcManagedObject.ice>

module Rpc
{
	interface ContentBrowser extends ManagedObject
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

	interface EngineVersionBrowser extends ManagedObject
	{
		ErrorCode next(int n, out EngineVersionItemSeq items);
	};

	interface Uploader extends ManagedObject
	{
		ErrorCode write(long offset, ["cpp:array"] ByteSeq bytes);
		ErrorCode finish(int crc32);
		void cancel();
	};

	interface Downloader extends ManagedObject
	{
		ErrorCode getSize(out long size);
		ErrorCode read(long offset, int size, out ByteSeq bytes);
		void finish();
	};

	interface ContentSubmitter extends ManagedObject
	{
		ErrorCode setTitle(string title);
		ErrorCode setPage(string page);
		ErrorCode setCategory(string category);
		ErrorCode setEngine(string name, string version);
		ErrorCode setCommand(string command);
		ErrorCode setParentId(string id);
		ErrorCode setDescription(string description);
		ErrorCode uploadImage(int index, out Uploader* uploader);
		ErrorCode uploadContent(out Uploader* uploader);
		void cancel();
		ErrorCode finish();
	};

	interface Session extends ManagedObject
	{
		void refresh();

		ErrorCode setPages(StringSeq pages);
		ErrorCode getPages(out StringSeq pages);

		ErrorCode setCategories(StringSeq categories);
		ErrorCode getCategories(out StringSeq categories);

		ErrorCode browseContent(string page, string category, string orderBy, out ContentBrowser* browser);

		ErrorCode submitContent(out ContentSubmitter* submitter);

		ErrorCode browseEngineVersions(out EngineVersionBrowser* browser);
		ErrorCode uploadEngineVersion(string name, string version, string info, out Uploader* uploader);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
	};
};

