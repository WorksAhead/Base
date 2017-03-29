#include "RpcCommentBrowserImpl.h"

#include <Ice/Ice.h>

RpcCommentBrowserImpl::RpcCommentBrowserImpl(ContextPtr context) : context_(context), destroyed_(false)
{
}

RpcCommentBrowserImpl::~RpcCommentBrowserImpl()
{
}

Rpc::ErrorCode RpcCommentBrowserImpl::init(const std::string& targetId, const std::string& user)
{
	std::ostringstream oss;
	oss << "SELECT * FROM Comments";

	if (!targetId.empty()) {
		oss << " WHERE TargetId=" << sqlText(targetId);
		if (!user.empty()) {
			oss << " AND User=" << sqlText(user);
		}
	}
	else if (!user.empty()) {
		oss << " WHERE User=" << sqlText(user);
	}

	oss << " ORDER BY Time DESC";

	s_.reset(new SQLite::Statement(*context_->center()->db(), oss.str()));

	return Rpc::ec_success;
}

void RpcCommentBrowserImpl::destroy(const Ice::Current& c)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	destroyed_ = true;

	try {
		c.adapter->remove(c.id);
	}
	catch (const Ice::ObjectAdapterDeactivatedException&) {
	}
}

Rpc::ErrorCode RpcCommentBrowserImpl::next(Ice::Int n, Rpc::CommentSeq& comments, const Ice::Current&)
{
	boost::recursive_mutex::scoped_lock lock(sync_);
	checkIsDestroyed();

	comments.clear();

	for (int i = 0; i < n; ++i)
	{
		if (!s_->executeStep()) {
			break;
		}

		Rpc::Comment c;
		c.id = s_->getColumn("Id").getText();
		c.targetId = s_->getColumn("TargetId").getText();
		c.user = s_->getColumn("User").getText();
		c.time = s_->getColumn("Time").getText();
		c.comment = s_->getColumn("Comment").getText();

		comments.push_back(c);
	}

	return Rpc::ec_success;
}

void RpcCommentBrowserImpl::checkIsDestroyed()
{
	if (destroyed_) {
		throw Ice::ObjectNotExistException(__FILE__, __LINE__);
	}
}

