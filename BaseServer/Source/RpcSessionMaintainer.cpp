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
}

void RpcSessionMaintainer::add(const ClientDownloader& downloader)
{
	Lock sync(*this);
	clientDownloaders_.push_back(downloader);
}

void RpcSessionMaintainer::runTimerTask()
{
	Lock sync(*this);
	maintainSessions();
	maintainClientDownloaders();
}

void RpcSessionMaintainer::maintainSessions()
{
	const IceUtil::Time timeout(IceUtil::Time::seconds(300));

	std::list<Session>::iterator p = sessions_.begin();

	while (p != sessions_.end())
	{
		try {
			if ((IceUtil::Time::now(IceUtil::Time::Monotonic) - p->second->timestamp()) > timeout)
			{
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

void RpcSessionMaintainer::maintainClientDownloaders()
{
	const IceUtil::Time timeout(IceUtil::Time::seconds(300));

	std::list<ClientDownloader>::iterator p = clientDownloaders_.begin();

	while (p != clientDownloaders_.end())
	{
		try {
			if ((IceUtil::Time::now(IceUtil::Time::Monotonic) - p->second->timestamp()) > timeout)
			{
				p->first->destroy();
				p = clientDownloaders_.erase(p);
			}
			else
			{
				++p;
			}
		}
		catch (const Ice::ObjectNotExistException&)
		{
			clientDownloaders_.erase(p);
		}
	}
}

