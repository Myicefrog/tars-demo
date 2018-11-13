#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
		int bind(string& ip, int& port);
		static void parseAddr(const string &sAddr, struct in_addr &stAddr);
	private:
		TC_EpollServer            *_epollServer;
		int _shutdown_sock;
		int _notify_sock;
		int _sock;
	};
private:
	std::vector<NetThread*>        _netThreads;
};

}

#endif
