#include "RpcSessionMaintainer.h"

RpcSessionMaintainer::RpcSessionMaintainer()
{
}

RpcSessionMaintainer::~RpcSessionMaintainer()
{
}

void RpcSessionMaintainer::add(const Session& session)
{
	 Lock sync(*this);
	 sessions_.push_back(session);
	 session.first->refresh();
	 std::cout << "create " << session.first->ice_getIdentity().name << std::endl;
}

void RpcSessionMaintainer::runTimerTask()
{
	Lock sync(*this);

	std::list<Session>::iterator p = sessions_.begin();

	const IceUtil::Time timeout(IceUtil::Time::seconds(30));

	while (p != sessions_.end())
	{
		try {
			if ((IceUtil::Time::now(IceUtil::Time::Monotonic) - p->second->timestamp()) > timeout)
			{
				std::cout << "destory " << p->first->ice_getIdentity().name << std::endl;
				p->first->destroy();
				p = sessions_.erase(p);
			}
			else
			{
				++p;
			}
		}
		catch (const Ice::ObjectNotExistException&)
		{
			p = sessions_.erase(p);
		}
	}
}

