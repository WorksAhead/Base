#ifndef BASESERVER_CONTEXT_HEADER_
#define BASESERVER_CONTEXT_HEADER_

#include "Center.h"
#include "RpcObjectManager.h"

#include <boost/shared_ptr.hpp>

#include <string>

class Context {
public:
	explicit Context(CenterPtr);
	~Context();

	CenterPtr center() const;

	RpcObjectManagerPtr objectManager() const;

	void setUser(const std::string&);
	std::string user() const;

	void setUserGroup(const std::string&);
	std::string userGroup() const;

private:
	CenterPtr center_;
	RpcObjectManagerPtr objectManager_;

	std::string user_;
	std::string userGroup_;
};

typedef boost::shared_ptr<Context> ContextPtr;

#endif // BASESERVER_CONTEXT_HEADER_

