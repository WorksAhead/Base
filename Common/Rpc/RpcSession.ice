#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>
#include <RpcManagedObject.ice>

module Rpc
{
	struct ContentItem
	{
		string id;
		string title;
	};

	struct ContentInfo
	{
		string id;
		string parentId;
		string title;
		string page;
		string category;
		string engineName;
		string engineVersion;
		string startup;
		int imageCount;
		string desc;
		string user;
		string upTime;
		string state;
	};

	sequence<ContentItem> ContentItemSeq;

	interface ContentBrowser extends ManagedObject
	{
		ErrorCode next(int n, out ContentItemSeq items);
	};

	struct EngineVersionInfo
	{
		string name;
		string version;
		string setup;
		string unsetup;
		string uptime;
		string info;
		string state;
	};

	sequence<EngineVersionInfo> EngineVersionSeq;

	interface EngineVersionBrowser extends ManagedObject
	{
		ErrorCode next(int n, out EngineVersionSeq items);
	};

	struct User
	{
		string username;
		string group;
		string regTime;
		string info;
	};

	sequence<User> UserSeq;

	interface UserBrowser extends ManagedObject
	{
		ErrorCode next(int n, out UserSeq items);
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
		void cancel();
	};

	interface ContentSubmitter extends ManagedObject
	{
		ErrorCode setTitle(string title);
		ErrorCode setPage(string page);
		ErrorCode setCategory(string category);
		ErrorCode setEngine(string name, string version);
		ErrorCode setStartup(string startup);
		ErrorCode setParentId(string id);
		ErrorCode setDescription(string description);
		ErrorCode uploadImage(int index, out Uploader* uploader);
		ErrorCode uploadContent(out Uploader* uploader);
		void cancel();
		ErrorCode finish();
	};

	interface EngineVersionSubmitter extends ManagedObject
	{
		ErrorCode setSetup(string setup);
		ErrorCode setUnSetup(string unsetup);
		ErrorCode setInfo(string info);
		ErrorCode uploadEngine(out Uploader* uploader);
		void cancel();
		ErrorCode finish();
	};

	interface Session extends ManagedObject
	{
		void refresh();

		ErrorCode getCurrentUser(out string user);
		ErrorCode getCurrentUserGroup(out string group);

		ErrorCode setPages(StringSeq pages);
		ErrorCode getPages(out StringSeq pages);

		ErrorCode setCategories(StringSeq categories);
		ErrorCode getCategories(out StringSeq categories);

		ErrorCode browseContent(string page, string category, out ContentBrowser* browser);
		ErrorCode getContentInfo(string id, out ContentInfo info);
		ErrorCode downloadContentImage(string id, int index, out Downloader* downloader);
		ErrorCode downloadContent(string id, out Downloader* downloader);
		ErrorCode submitContent(out ContentSubmitter* submitter);
		ErrorCode updateContent(string id, out ContentSubmitter* submitter);
		ErrorCode removeContent(string id);

		ErrorCode browseEngineVersions(out EngineVersionBrowser* browser);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
		ErrorCode submitEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode updateEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode getEngineVersion(string name, string version, out EngineVersionInfo engineVersion);

		ErrorCode browseUsers(out UserBrowser* browser);
		ErrorCode setUserGroup(string username, string group);
		ErrorCode resetUserPassword(string username, string password);
		ErrorCode removeUser(string username);
	};
};

