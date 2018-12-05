#include "ObjectProxy.h"

namespace tars
{

ObjectProxy::ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll)
:_communicatorEpoll(pCommunicatorEpoll)
{
	string _host = "127.0.0.1";
	uint16_t _port = 9877;

	_trans.reset(new Transceiver(this, _host, _port));
}

ObjectProxy::~ObjectProxy()
{
}


void ObjectProxy::invoke(ReqMessage * msg)
{
//_endpointManger->selectAdapterProxy
//pAdapterProxy = getNextValidProxy();
//adapterProxy->checkActive(true);
//_trans->reconnect();

	_trans->connect();
	cout<<"_trans->fd() is "<<_trans->fd()<<endl;
	FDInfo                   _fdInfo;
	    _fdInfo.iType = FDInfo::ET_C_NET;
    _fdInfo.p     = NULL;
    _fdInfo.fd    = -1;
	getCommunicatorEpoll()->addFd(_trans->fd(), &_fdInfo, EPOLLIN|EPOLLOUT);
}

}
