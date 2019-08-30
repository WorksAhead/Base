CPPFLAGS=-g -c -ansi -W -std=c++11 -I../Common -I../Common/Rpc -I../Ext/SQLiteCpp/include -I../Ext/sqlite3

BaseServer: BaseServer.o Center.o Context.o Main.o PathUtils.o RpcClientBrowserImpl.o RpcClientDownloaderImpl.o RpcClientSubmitterImpl.o RpcCommentBrowserImpl.o RpcContentBrowserImpl.o RpcContentSubmitterImpl.o RpcEngineVersionBrowserImpl.o RpcEngineVersionDownloaderImpl.o RpcEngineVersionSubmitterImpl.o RpcExtraBrowserImpl.o RpcExtraSubmitterImpl.o RpcFileDownloaderImpl.o RpcFileUploaderImpl.o RpcObjectManager.o RpcSessionImpl.o RpcSessionMaintainer.o RpcStartImpl.o RpcUserBrowserImpl.o sqlite3.o Column.o Database.o Statement.o Transaction.o Base64.o Rijndael.o Crc.o ErrorMessage.o RpcDownloader.o RpcErrorCode.o RpcManagedObject.o RpcSession.o RpcStart.o RpcTypedefs.o RpcUploader.o
	g++ -g -o BaseServer BaseServer.o Center.o Context.o Main.o PathUtils.o RpcClientBrowserImpl.o RpcClientDownloaderImpl.o RpcClientSubmitterImpl.o RpcCommentBrowserImpl.o RpcContentBrowserImpl.o RpcContentSubmitterImpl.o RpcEngineVersionBrowserImpl.o RpcEngineVersionDownloaderImpl.o RpcEngineVersionSubmitterImpl.o RpcExtraBrowserImpl.o RpcExtraSubmitterImpl.o RpcFileDownloaderImpl.o RpcFileUploaderImpl.o RpcObjectManager.o RpcSessionImpl.o RpcSessionMaintainer.o RpcStartImpl.o RpcUserBrowserImpl.o sqlite3.o Column.o Database.o Statement.o Transaction.o Base64.o Rijndael.o Crc.o ErrorMessage.o RpcDownloader.o RpcErrorCode.o RpcManagedObject.o RpcSession.o RpcStart.o RpcTypedefs.o RpcUploader.o -lIce -lcurl -lboost_filesystem -lboost_system -ldl -lpthread

BaseServer.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/BaseServer.cpp

Center.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/Center.cpp

Context.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/Context.cpp

Main.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/Main.cpp

PathUtils.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/PathUtils.cpp

RpcClientBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcClientBrowserImpl.cpp

RpcClientDownloaderImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcClientDownloaderImpl.cpp

RpcClientSubmitterImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcClientSubmitterImpl.cpp

RpcCommentBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcCommentBrowserImpl.cpp

RpcContentBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcContentBrowserImpl.cpp

RpcContentSubmitterImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcContentSubmitterImpl.cpp

RpcEngineVersionBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcEngineVersionBrowserImpl.cpp

RpcEngineVersionDownloaderImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcEngineVersionDownloaderImpl.cpp

RpcEngineVersionSubmitterImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcEngineVersionSubmitterImpl.cpp

RpcExtraBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcExtraBrowserImpl.cpp

RpcExtraSubmitterImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcExtraSubmitterImpl.cpp

RpcFileDownloaderImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcFileDownloaderImpl.cpp

RpcFileUploaderImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcFileUploaderImpl.cpp

RpcObjectManager.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcObjectManager.cpp

RpcSessionImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcSessionImpl.cpp

RpcSessionMaintainer.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcSessionMaintainer.cpp

RpcStartImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcStartImpl.cpp

RpcUserBrowserImpl.o:
	g++ $(CPPFLAGS) ../BaseServer/Source/RpcUserBrowserImpl.cpp

sqlite3.o:
	gcc -g -c ../Ext/sqlite3/sqlite3.c

Column.o:
	g++ $(CPPFLAGS) ../Ext/SQLiteCpp/src/Column.cpp

Database.o:
	g++ $(CPPFLAGS) ../Ext/SQLiteCpp/src/Database.cpp

Statement.o:
	g++ $(CPPFLAGS) ../Ext/SQLiteCpp/src/Statement.cpp

Transaction.o:
	g++ $(CPPFLAGS) ../Ext/SQLiteCpp/src/Transaction.cpp

Base64.o:
	g++ $(CPPFLAGS) ../Common/Security/Base64.cpp

Rijndael.o:
	g++ $(CPPFLAGS) ../Common/Security/Rijndael.cpp

Crc.o:
	g++ $(CPPFLAGS) ../Common/Crc.cpp

ErrorMessage.o:
	g++ $(CPPFLAGS) ../Common/ErrorMessage.cpp

RpcDownloader.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcDownloader.cpp

RpcErrorCode.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcErrorCode.cpp

RpcManagedObject.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcManagedObject.cpp

RpcSession.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcSession.cpp

RpcStart.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcStart.cpp

RpcTypedefs.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcTypedefs.cpp

RpcUploader.o:
	g++ $(CPPFLAGS) ../Common/Rpc/RpcUploader.cpp


.PHONY: clean

clean:
	rm *.o BaseServer


