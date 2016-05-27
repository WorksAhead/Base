#ifndef CONTEXT_HEADER_
#define CONTEXT_HEADER_

#include <RpcSession.h>

#include <functional>
#include <memory>

// forward declaration
class ASyncTask;

struct Context
{
	Rpc::SessionPrx session;
	std::function<void(ASyncTask*)> addTask;
};

typedef std::shared_ptr<Context> ContextPtr;

#endif // CONTEXT_HEADER_

