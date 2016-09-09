#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>
#include <RpcManagedObject.ice>

module Rpc
{
	interface Downloader extends ManagedObject
	{
		ErrorCode getSize(out long size);
		ErrorCode read(long offset, int size, out ByteSeq bytes);
		void finish();
		void cancel();
	};
};

