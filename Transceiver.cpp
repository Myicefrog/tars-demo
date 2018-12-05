#include "Transceiver.h"
#include "ObjectProxy.h"

namespace tars
{

Transceiver::Transceiver(ObjectProxy *objectProxy,const string& ip, const uint16_t& port)
: _fd(-1)
{
    _fdInfo.iType = FDInfo::ET_C_NET;
    _fdInfo.p     = (void *)this;
    _fdInfo.fd    = -1;

	_ip = ip;
	_port = port;

	_objectProxy = objectProxy;
}

Transceiver::~Transceiver()
{
    close();
}

void Transceiver::close()
{
    if(!isValid()) return;

    getObjProxy()->getCommunicatorEpoll()->delFd(_fd,&_fdInfo,EPOLLIN|EPOLLOUT);

    NetworkUtil::closeSocketNoThrow(_fd);

    _fd = -1;

}

void Transceiver::connect()
{
	int fd = -1;

    fd = NetworkUtil::createSocket(false);

    NetworkUtil::setBlock(fd, false);

    struct sockaddr_in     _addr;

    NetworkUtil::getAddress(_ip, _port, _addr);

    bool bConnected = NetworkUtil::doConnect(fd, _addr);

    if(bConnected)
    {
        cout<<"bConnect successful"<<endl;
    }
    else
    {
        cout<<"bConnect connecting"<<endl;
    }

	_fd = fd;
    //值得学习地方：设置网络qos的dscp标志
    //int iQos;
    //::setsockopt(fd,SOL_IP,IP_TOS,&iQos,sizeof(iQos));

	cout<<"EPOLLIN|EPOLLOUT is "<<(EPOLLIN|EPOLLOUT)<<endl;

	getObjProxy()->getCommunicatorEpoll()->addFd(fd, &_fdInfo, EPOLLIN|EPOLLOUT);
}

int Transceiver::doRequest()
{
	return 0;	
}

int Transceiver::sendRequest(const char * pData, size_t iSize, bool forceSend)
{
	int iRet = this->send(pData,iSize,0);

	if(iRet < 0)
    {
        return eRetError;
    }
	return iRet;
}

int Transceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
	int iRet = ::send(_fd, buf, len, flag);	
	if (iRet < 0 && errno != EAGAIN)
    {
		cout<<"Transceiver::send fail"<<endl;
		return iRet;

	}

	if (iRet < 0 && errno == EAGAIN)
        iRet = 0;

	return iRet;

}



}
