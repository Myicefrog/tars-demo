#include "tc_epoll_server.h"

using namespace std;

namespace tars
{

TC_EpollServer::TC_EpollServer(unsigned int iNetThreadNum)
{
	for (size_t i = 0; i < _netThreadNum; ++i)
	{
		TC_EpollServer::NetThread* netThreads = new TC_EpollServer::NetThread(this);
		_netThreads.push_back(netThreads);
       	}
}

TC_EpollServer::~TC_EpollServer()
{
}

TC_EpollServer::NetThread::NetThread(TC_EpollServer *epollServer)
{
	//_shutdown.createSocket();
	//_notify.createSocket();
	int iSocketType = SOCK_STREAM;
       	int iDomain = AF_INET;
	_shutdown_sock = socket(iDomain, iSocketType, 0);
	_notify_sock = socket(iDomain, iSocketType, 0);
}
};

}

#endif
