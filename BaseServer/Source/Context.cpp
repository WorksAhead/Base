#include "Context.h"

Context::Context(CenterPtr center) : center_(center)
{
	objectManager_.reset(new RpcObjectManager);
}

Context::~Context()
{
}

CenterPtr Context::center() const
{
	return center_;
}

RpcObjectManagerPtr Context::objectManager() const
{
	return objectManager_;
}

void Context::setUser(const std::string& user)
{
	user_ = user;
}

std::string Context::user() const
{
	return user_;
}

void Context::setUserGroup(const std::string& userGroup)
{
	userGroup_ = userGroup;
}

std::string Context::userGroup() const
{
	return userGroup_;
}

