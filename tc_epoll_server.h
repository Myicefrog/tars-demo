#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <vector>

#include "tc_epoller.h"

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

		void createEpoll(uint32_t iIndex = 0);

		enum
        	{
            		ET_LISTEN = 1,
            		ET_CLOSE  = 2,
            		ET_NOTIFY = 3,
            		ET_NET    = 0,
        	};

	private:
		TC_EpollServer            *_epollServer;

		int _shutdown_sock;
		int _notify_sock;

		int _sock;

		TC_Epoller                  _epoller;
	};

public:
	vector<TC_EpollServer::NetThread*> getNetThread() { return _netThreads; }
private:
	std::vector<NetThread*>        _netThreads;
};

}

#endif
