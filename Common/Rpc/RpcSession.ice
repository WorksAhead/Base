#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>
#include <RpcManagedObject.ice>
#include <RpcDownloader.ice>
#include <RpcUploader.ice>

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

	struct ExtraInfo
	{
		string id;
		string title;
		string setup;
		string user;
		string uptime;
		string info;
		string state;
	};

	sequence<ExtraInfo> ExtraInfoSeq;

	interface ExtraBrowser extends ManagedObject
	{
		ErrorCode next(int n, out ExtraInfoSeq items);
	};

	struct User
	{
		string username;
		string group;
		string info;
	};

	sequence<User> UserSeq;

	interface UserBrowser extends ManagedObject
	{
		ErrorCode next(int n, out UserSeq items);
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

	interface ExtraSubmitter extends ManagedObject
	{
		ErrorCode setTitle(string title);
		ErrorCode setSetup(string setup);
		ErrorCode setInfo(string info);
		ErrorCode uploadExtra(out Uploader* uploader);
		void cancel();
		ErrorCode finish();
	};

	struct ClientInfo
	{
		string version;
		string uptime;
		string info;
		string state;
	};

	sequence<ClientInfo> ClientInfoSeq;

	interface ClientBrowser extends ManagedObject
	{
		ErrorCode next(int n, out ClientInfoSeq items);
	};

	interface ClientSubmitter extends ManagedObject
	{
		ErrorCode setInfo(string info);
		ErrorCode uploadClient(out Uploader* uploader);
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

		ErrorCode browseEngineVersions(bool all, out EngineVersionBrowser* browser);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
		ErrorCode submitEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode updateEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode getEngineVersion(string name, string version, out EngineVersionInfo engineVersion);

		ErrorCode browseExtra(out ExtraBrowser* browser);
		ErrorCode getExtraInfo(string id, out ExtraInfo info);
		ErrorCode downloadExtra(string id, out Downloader* downloader);
		ErrorCode submitExtra(out ExtraSubmitter* submitter);
		ErrorCode updateExtra(string id, out ExtraSubmitter* submitter);
		ErrorCode removeExtra(string id);

		ErrorCode browseClient(out ClientBrowser* browser);
		ErrorCode getClientInfo(string version, out ClientInfo info);
		ErrorCode submitClient(string version, out ClientSubmitter* submitter);
		ErrorCode updateClient(string version, out ClientSubmitter* submitter);
		ErrorCode removeClient(string version);

		ErrorCode browseUsers(out UserBrowser* browser);
		ErrorCode setUserGroup(string username, string group);
		ErrorCode removeUser(string username);
	};
};

