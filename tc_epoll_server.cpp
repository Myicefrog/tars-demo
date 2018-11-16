#include "tc_epoll_server.h"

#include <string.h>
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
#include <sys/epoll.h>

using namespace std;

namespace tars
{
#define H64(x) (((uint64_t)x) << 32)

TC_EpollServer::TC_EpollServer(unsigned int iNetThreadNum)
{
	for (size_t i = 0; i < iNetThreadNum; ++i)
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

	parseAddr(ip, bindAddr.sin_addr);	

	//如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间
	int iReuseAddr = 1;

	setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int));
	
	if(::bind(_sock, (struct sockaddr *)(&bindAddr), sizeof(bindAddr)) < 0)
	{
		cout<<"bind error"<<endl;
	}

	cout<<"already bind"<<endl;

	int iConnBackLog = 1024;	
	if (::listen(_sock, iConnBackLog) < 0)
	{
		cout<<"listen error"<<endl;
	}
	
	cout<<"alreay listen fd "<<_sock<<endl;

	int flag = 1;
	if(setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, int(sizeof(int))) == -1)
	{
		cout<<"setKeepAlive] error"<<endl;
	}

	flag=1;
	if(setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == -1)
	{
		cout<<"[TC_Socket::setTcpNoDelay] error"<<endl;
	}

	linger stLinger;
	stLinger.l_onoff = 1;  //在close socket调用后, 但是还有数据没发送完毕的时候容许逗留
	stLinger.l_linger = 0; //容许逗留的时间为0秒

	if(setsockopt(_sock, SOL_SOCKET, SO_LINGER, (const void *)&stLinger, sizeof(linger)) == -1)
	{
		cout<<"[TC_Socket::setNoCloseWait] error"<<endl;
	}	


	int val = 0;
	bool bBlock = false;

	if ((val = fcntl(_sock, F_GETFL, 0)) == -1)
	{
		cout<<"[TC_Socket::setblock] fcntl [F_GETFL] error"<<endl;
	}

	if(!bBlock)
	{
		val |= O_NONBLOCK;
	}
	else
	{
		val &= ~O_NONBLOCK;
	}

	if (fcntl(_sock, F_SETFL, val) == -1)
	{
		cout<<"[TC_Socket::setblock] fcntl [F_SETFL] error"<<endl;
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

void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
{
	_epoller.create(10240);
	
	_epoller.add(_shutdown_sock, H64(ET_CLOSE), EPOLLIN);
        _epoller.add(_notify_sock, H64(ET_NOTIFY), EPOLLIN);	

	cout<<"H64(ET_LISTEN) | _sock is "<< (H64(ET_LISTEN) | _sock) <<endl;

	_epoller.add(_sock, H64(ET_LISTEN) | _sock, EPOLLIN);	
}

void TC_EpollServer::NetThread::run()
{

	cout<<"NetThread run"<<endl;

	while(true)
	{
		int iEvNum = _epoller.wait(2000);
	
		for(int i = 0; i < iEvNum; ++i)
		{
			const epoll_event &ev = _epoller.get(i);	

			uint32_t h = ev.data.u64 >> 32;
			
			cout<<"ev.data.u64 is "<<ev.data.u64<<endl;

			cout<<"ev.data.u32 is "<<ev.data.u32<<endl;

			cout<<"ev.data.fd is "<<ev.data.fd<<endl;
			
			cout<<"epoll h is "<<h<<endl;

			switch(h)
			{
			case ET_LISTEN:
				cout<<"ET_LISTEN"<<endl;
				{
					if(ev.events & EPOLLIN)
					{
						bool ret;
						do
						{
							ret = accept(ev.data.u32);
						}while(ret);
					}
				}
				break;
			case ET_CLOSE:
				cout<<"ET_CLOSE"<<endl;
				break;
			case ET_NOTIFY:
				cout<<"ET_NOTIFY"<<endl;	
				processPipe();
				break;
			case ET_NET:
				cout<<"ET_NET"<<endl;
				processNet(ev);
				break;
			default:
				assert(true);
			}
		}
	}
}

bool TC_EpollServer::NetThread::accept(int fd)
{
	struct sockaddr_in stSockAddr;

	socklen_t iSockAddrSize = sizeof(sockaddr_in);	

	int iifd;

	while((iifd = ::accept(_sock, (struct sockaddr *) &stSockAddr, &iSockAddrSize)) < 0 && errno == EINTR);

	cout<<"accept _sock is "<<_sock<<" fd is "<<fd<<endl;

	cout<<"accept iifd is "<<iifd<<endl;
	
	if(iifd > 0)
	{
		_listen_connect_id[fd] = iifd;	

		string  ip;
		
		uint16_t port;

		char sAddr[INET_ADDRSTRLEN] = "\0";

		struct sockaddr_in *p = (struct sockaddr_in *)&stSockAddr;

		inet_ntop(AF_INET, &p->sin_addr, sAddr, sizeof(sAddr));

		ip      = sAddr;
		port    = ntohs(p->sin_port);

		//setblock
		int val = 0;
		int bBlock = false;
		if((val = fcntl(iifd, F_GETFL, 0)) == -1)	
		{
			cout<<"F_GETFL error"<<endl;
		}	
		
		if(!bBlock)
		{
			val |= O_NONBLOCK;
		}
		else
		{
			val &= ~O_NONBLOCK;
		}

		if(fcntl(iifd, F_SETFL, val) == -1)
		{
			cout<<"F_SETFL error"<<endl;
		}

		//keepAlive
		int flag = 1;
    		if(setsockopt(iifd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, int(sizeof(int))) == -1)
		{
			cout<<"[TC_Socket::setKeepAlive] error"<<endl;
		}

		//nodelay
		if(setsockopt(iifd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == -1)
		{
			cout<<"[TC_Socket::setTcpNoDelay] error"<<endl;
		}

		//closeWait
		linger stLinger;
		stLinger.l_onoff  = 0;
		stLinger.l_linger = 0;

		if(setsockopt(iifd, SOL_SOCKET, SO_LINGER, (const void *)&stLinger, sizeof(linger)) == -1)
		{
			cout<<"[TC_Socket::setCloseWaitDefault] error"<<endl;
		}

		_epoller.add(iifd, 0, EPOLLIN | EPOLLOUT);

	}
	else
	{
		if(errno == EAGAIN)
		{
			return false;
		}
		return true;
	}
	return true;
}

void TC_EpollServer::NetThread::processNet(const epoll_event &ev)
{
	uint32_t uid = ev.data.u32;	

	int fd = uid;

	//int fd = _listen_connect_id[uid];

	//cout<<"processNet uid is "<<uid<<" connect fd is "<<fd<<endl;

	cout<<"processNet uid is "<<uid<<endl;

	if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
	{
		cout<<"should delet connection"<<endl;
		return;
	}

	if(ev.events & EPOLLIN)
	{
		while(true)
		{
			char buffer[32*1024];
			int iBytesReceived = 0;
			
			iBytesReceived = ::read(fd, (void*)buffer, sizeof(buffer));

			cout<<"iBytesReceived is "<<iBytesReceived<<endl;

			cout<<"::read fd is "<<fd<<endl;

			cout<<"receive buffer is "<<buffer<<endl;
			
			if(iBytesReceived < 0)
			{
				if(errno == EAGAIN)
				{
					break;
				}
				else
				{
					cout<<"client close"<<endl;
					return ;
				}
			}
			else if( iBytesReceived == 0 )
			{
				cout<<"1 client close"<<endl;
				return ;
			}

			_recvbuffer.append(buffer, iBytesReceived);

			response = "hello";
			
			_epoller.mod(_notify_sock, H64(ET_NOTIFY), EPOLLOUT);
		}
	}

	if (ev.events & EPOLLOUT)
	{
		cout<<"need to send data"<<endl;
	}	
}

void TC_EpollServer::NetThread::processPipe()
{	
	cout<<"processPipe"<<endl;

	cout<<"send ifd is "<<ifd<<endl;

	cout<<"response is "<<response<<endl;

	int bytes = ::send(ifd, response.c_str(), response.size(), 0);

	cout<<"send byte is "<<bytes<<endl;
}


}

