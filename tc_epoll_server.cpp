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
	_shutdown.createSocket();
	_notify.createSocket();
	
	_response.response="";
	_response.uid = 0;
}

TC_EpollServer::NetThread::~NetThread()
{
}

int  TC_EpollServer::NetThread::bind(string& ip, int& port)
{

	int type = AF_INET;
	
	_bind_listen.createSocket(SOCK_STREAM, type);

	_bind_listen.bind(ip,port);

	cout<<"bind fd is "<<_bind_listen.getfd()<<endl;

        _bind_listen.listen(1024);

	cout<<"listen fd is "<<_bind_listen.getfd()<<endl;

        _bind_listen.setKeepAlive();
        _bind_listen.setTcpNoDelay();
        //不要设置close wait否则http服务回包主动关闭连接会有问题
        _bind_listen.setNoCloseWait();

	_bind_listen.setblock(false);

	return _bind_listen.getfd();

}


void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
{
	int _total = 200000;
	
	_epoller.create(10240);
	
	cout<<"H64(ET_CLOSE) is "<<H64(ET_CLOSE)<<endl;
	cout<<"H64(ET_NOTIFY) is "<<H64(ET_NOTIFY)<<endl;

	_epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);
        _epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);	

	cout<<"H64(ET_LISTEN) | _sock is "<< (H64(ET_LISTEN) | _bind_listen.getfd()) <<endl;

	_epoller.add(_bind_listen.getfd(), H64(ET_LISTEN) | _bind_listen.getfd(), EPOLLIN);	

	for(uint32_t i = 1; i <= _total; i++)
	{

		_free.push_back(i);

		++_free_size;
	}
}

void TC_EpollServer::NetThread::run()
{

	cout<<"NetThread run"<<endl;

	while(true)
	{
		int iEvNum = _epoller.wait(2000);
	
		cout<<"iEvNum is "<<iEvNum<<endl;
	
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
	cout<<"listen fd is "<<fd<<endl;

	struct sockaddr_in stSockAddr;

	socklen_t iSockAddrSize = sizeof(sockaddr_in);	

	TC_Socket cs;
    	cs.setOwner(false);

    	//接收连接
    	TC_Socket s;
    	s.init(fd, false, AF_INET);

	int iRetCode = s.accept(cs, (struct sockaddr *) &stSockAddr, iSockAddrSize);

	if (iRetCode > 0)
    	{
		string  ip;

        	uint16_t port;

        	char sAddr[INET_ADDRSTRLEN] = "\0";

        	struct sockaddr_in *p = (struct sockaddr_in *)&stSockAddr;

        	inet_ntop(AF_INET, &p->sin_addr, sAddr, sizeof(sAddr));

        	ip      = sAddr;
        	port    = ntohs(p->sin_port);

		cout<<"accept ip is "<<ip<<" port is "<<port<<endl;


		cs.setblock(false);
        	cs.setKeepAlive();
        	cs.setTcpNoDelay();
        	cs.setCloseWaitDefault();


		uint32_t uid = _free.front();

		_free.pop_front();

		--_free_size;

		_listen_connect_id[uid] = cs.getfd();	

		_epoller.add(cs.getfd(), uid, EPOLLIN | EPOLLOUT);

	}
/*
	int iifd;

	while((iifd = ::accept(_sock, (struct sockaddr *) &stSockAddr, &iSockAddrSize)) < 0 && errno == EINTR);

	cout<<"accept _sock is "<<_sock<<" fd is "<<fd<<endl;

	cout<<"accept iifd is "<<iifd<<endl;
	
	if(iifd > 0)
	{

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


		uint32_t uid = _free.front();

		_free.pop_front();

		--_free_size;

		_listen_connect_id[uid] = iifd;	

		_epoller.add(iifd, uid, EPOLLIN | EPOLLOUT);

	}
*/
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

	int fd = _listen_connect_id[uid];

	cout<<"processNet uid is "<<uid<<" fd is "<<fd<<endl;

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

			_response.response = "hello";
			_response.uid = uid;
			
			_epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
		}
	}

	if (ev.events & EPOLLOUT)
	{
		cout<<"need to send data"<<endl;
	}	
}

void TC_EpollServer::NetThread::processPipe()
{	
        uint32_t uid = _response.uid;

        int fd = _listen_connect_id[uid];

        //cout<<"processNet uid is "<<uid<<" connect fd is "<<fd<<endl;

        cout<<"processPipe uid is "<<uid<<" fd is "<<fd<<endl;

	cout<<"response is "<<_response.response<<endl;

	int bytes = ::send(fd, _response.response.c_str(), _response.response.size(), 0);

	cout<<"send byte is "<<bytes<<endl;
}


}

