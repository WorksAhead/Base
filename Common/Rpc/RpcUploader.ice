#pragma once

#include <RpcErrorCode.ice>
#include <RpcTypedefs.ice>
#include <RpcManagedObject.ice>

module Rpc
{
	interface Uploader extends ManagedObject
	{
		ErrorCode write(long offset, ["cpp:array"] ByteSeq bytes);
		ErrorCode finish(int crc32);
		void cancel();
	};
};

