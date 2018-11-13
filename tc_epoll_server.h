#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace tars
{

class TC_EpollServer
{
public:
	TC_EpollServer(unsigned int iNetThreadNum = 1);
	~TC_EpollServer();
	class NetThread
	{
	public:
		NetThread(TC_EpollServer *epollServer);
		virtual ~NetThread();	
	private:
		TC_EpollServer            *_epollServer;
		int _shutdown_sock;
		int _notify_sock;
	};
private:
	std::vector<NetThread*>        _netThreads;
};

}

#endif
