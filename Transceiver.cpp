#include "Transceiver.h"
#include "AdapterProxy.h"

namespace tars
{

Transceiver::Transceiver(AdapterProxy * pAdapterProxy,const EndpointInfo &ep)
: _adapterProxy(pAdapterProxy)
, _ep(ep)
, _fd(-1)
{
    _fdInfo.iType = FDInfo::ET_C_NET;
    _fdInfo.p     = (void *)this;
    _fdInfo.fd    = -1;
}

Transceiver::~Transceiver()
{
    close();
}

void Transceiver::close()
{
    if(!isValid()) return;

    _adapterProxy->getObjProxy()->getCommunicatorEpoll()->delFd(_fd,&_fdInfo,EPOLLIN|EPOLLOUT);

    NetworkUtil::closeSocketNoThrow(_fd);

    _fd = -1;

}

}
