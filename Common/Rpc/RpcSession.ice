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
		string state;
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
		string video;
		string desc;
		string user;
		string upTime;
		int displayPriority;
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
		int displayPriority;
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
		string parentId;
		string title;
		string category;
		string setup;
		string user;
		string uptime;
		string info;
		int displayPriority;
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
		ErrorCode getId(out string id);
		ErrorCode setTitle(string title);
		ErrorCode setPage(string page);
		ErrorCode setCategory(string category);
		ErrorCode setEngine(string name, string version);
		ErrorCode setStartup(string startup);
		ErrorCode setParentId(string id);
		ErrorCode setVideo(string video);
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
		ErrorCode setParentId(string id);
		ErrorCode setTitle(string title);
		ErrorCode setSetup(string setup);
		ErrorCode setCategory(string category);
		ErrorCode setInfo(string info);
		ErrorCode uploadImage(out Uploader* uploader);
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

	struct Comment
	{
		string id;
		string targetId;
		string user;
		string time;
		string comment;
	};

	sequence<Comment> CommentSeq;

	interface CommentBrowser extends ManagedObject
	{
		ErrorCode next(int n, out CommentSeq items);
	};

	interface Session extends ManagedObject
	{
		void refresh();

		ErrorCode getCurrentUser(out string user);
		ErrorCode getCurrentUserGroup(out string group);

		ErrorCode setPages(StringSeq pages);
		ErrorCode getPages(out StringSeq pages);

		ErrorCode setContentCategories(StringSeq categories);
		ErrorCode getContentCategories(out StringSeq categories);

		ErrorCode setExtraCategories(StringSeq categories);
		ErrorCode getExtraCategories(out StringSeq categories);

		ErrorCode setUniformInfo(string key, string value);
		ErrorCode getUniformInfo(string key, out string value);

		ErrorCode browseContent(string page, string category, string search, out ContentBrowser* browser);
		ErrorCode browseContentByParentId(string parentId, out ContentBrowser* browser);
		ErrorCode getContentInfo(string id, out ContentInfo info);
		ErrorCode downloadContentImage(string id, int index, out Downloader* downloader);
		ErrorCode downloadContent(string id, out Downloader* downloader);
		ErrorCode submitContent(out ContentSubmitter* submitter);
		ErrorCode copyContent(string id, out ContentSubmitter* submitter);
		ErrorCode editContent(string id, out ContentSubmitter* submitter);
		ErrorCode changeContentState(string id, string state);
		ErrorCode changeContentDisplayPriority(string id, int displayPriority);

		ErrorCode browseEngineVersions(bool all, out EngineVersionBrowser* browser);
		ErrorCode downloadEngineVersion(string name, string version, out Downloader* downloader);
		ErrorCode removeEngineVersion(string name, string version);
		ErrorCode submitEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode updateEngineVersion(string name, string version, out EngineVersionSubmitter* submitter);
		ErrorCode getEngineVersion(string name, string version, out EngineVersionInfo engineVersion);
		ErrorCode changeEngineVersionDisplayPriority(string name, string version, int displayPriority);

		ErrorCode browseExtra(string category, string search, out ExtraBrowser* browser);
		ErrorCode browseExtraByParentId(string parentId, out ExtraBrowser* browser);
		ErrorCode getExtraInfo(string id, out ExtraInfo info);
		ErrorCode downloadExtraImage(string id, out Downloader* downloader);
		ErrorCode downloadExtra(string id, out Downloader* downloader);
		ErrorCode submitExtra(out ExtraSubmitter* submitter);
		ErrorCode updateExtra(string id, out ExtraSubmitter* submitter);
		ErrorCode changeExtraState(string id, string state);
		ErrorCode changeExtraDisplayPriority(string id, int displayPriority);

		ErrorCode browseClient(out ClientBrowser* browser);
		ErrorCode getClientInfo(string version, out ClientInfo info);
		ErrorCode submitClient(string version, out ClientSubmitter* submitter);
		ErrorCode updateClient(string version, out ClientSubmitter* submitter);
		ErrorCode removeClient(string version);

		ErrorCode browseUsers(out UserBrowser* browser);
		ErrorCode setUserGroup(string username, string group);
		ErrorCode removeUser(string username);

		ErrorCode browseComment(string targetId, string user, out CommentBrowser* browser);
		ErrorCode getComment(string targetId, out string comment);
		ErrorCode addComment(string targetId, string comment);
		ErrorCode editComment(string id, string comment);
		ErrorCode removeComment(string id);

		ErrorCode queryDownloadCount(string targetId, out int count);

		ErrorCode isUserOnline(string userName, out bool result);
		ErrorCode onlineUserCount(out int count);
	};
};

