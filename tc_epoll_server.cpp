#include "tc_epoll_server.h"

#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cassert>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
: _epollServer(epollServer)
{
	//_shutdown.createSocket();
	//_notify.createSocket();
	int iSocketType = SOCK_STREAM;
       	int iDomain = AF_INET;
	_shutdown_sock = socket(iDomain, iSocketType, 0);
	_notify_sock = socket(iDomain, iSocketType, 0);
	
	if(_shutdown_sock < 0)
	{
		cout<<"_shutdown_sock  invalid"<<endl;
	}
	if(_notify_sock < 0)
	{
		cout<<"_notify_sock invalid"<<endl;
	}
}

TC_EpollServer::NetThread::~NetThread()
{
}

int  TC_EpollServer::NetThread::bind(string& ip, int& port)
{
	int iSocketType = SOCK_STREAM;
	int iDomain = AF_INET;
	_sock = socket(iDomain, iSocketType, 0);
	
	if(_sock < 0)
	{
		cout<<"bind _sock invalid"<<endl;
	}

	struct sockaddr_in bindAddr;
	
	bzero(&bindAddr, sizeof(bindAddr));
	
	bindAddr.sin_family   = iDomain;
	bindAddr.sin_port     = htons(port);

	parseAddr(sServerAddr, bindAddr.sin_addr);	

	//如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间
	int iReuseAddr = 1;

	setSockOpt(SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int), SOL_SOCKET);
	
	if(::bind(_sock, pstBindAddr, iAddrLen) < 0)
	{
		cout<<"bind error"<<endl;
	}
	
	return _sock;
}

void TC_EpollServer::NetThread::parseAddr(const string &sAddr, struct in_addr &stSinAddr)
{
	//将点分十进制转为二进制整数
	int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
	if(iRet < 0)
	{
		cout<<"parseAddr iRet error"<<endl;
	}
	else if(iRet == 0)
	{
		struct hostent stHostent;
		struct hostent *pstHostent;
		char buf[2048] = "\0";
		int iError;

		gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);

		if (pstHostent == NULL)
		{
			cout<<"gethostbyname_r error! :"<<endl;
		}
		else
		{
			stSinAddr = *(struct in_addr *) pstHostent->h_addr;
		}	
	}
}

};

}

#endif
