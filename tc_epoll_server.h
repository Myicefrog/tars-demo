#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>


#include "tc_epoller.h"
#include "tc_socket.h"

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

		void run();		

		void createEpoll(uint32_t iIndex = 0);

		bool accept(int fd);

		void processNet(const epoll_event &ev);

		void processPipe();

		enum
        	{
            		ET_LISTEN = 1,
            		ET_CLOSE  = 2,
            		ET_NOTIFY = 3,
            		ET_NET    = 0,
        	};

		struct
		{
			string response;
			uint32_t uid;	
		}_response;

	private:
		TC_EpollServer            *_epollServer;

		TC_Socket                   _shutdown;
	 	TC_Socket                   _notify;

		TC_Socket                   _bind_listen;

		TC_Epoller                  _epoller;

		string              _recvbuffer;

		string              response;

		map<int,int>        _listen_connect_id;

		list<uint32_t>                  _free;

		volatile size_t                 _free_size;
	};

public:
	vector<TC_EpollServer::NetThread*> getNetThread() { return _netThreads; }
private:
	std::vector<NetThread*>        _netThreads;
};

}

#endif
