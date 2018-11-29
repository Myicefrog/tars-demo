#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServerPtr _epollServer = make_shared<TC_EpollServer>(3);

    //vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

    string ip = "127.0.0.1";

    int port = 9877;

    TC_EpollServer::BindAdapterPtr lsPtr = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

    lsPtr->setEndpoint(ip,port);

    _epollServer->bind(lsPtr);
    
    vector<TC_EpollServer::HandlePtr>          handles;

	int handleNum = 4;

	for (int32_t i = 0; i < handleNum; ++i)
	{
		TC_EpollServer::HandlePtr handle = make_shared<TC_EpollServer::Handle>();
		handle->setEpollServer(_epollServer.get());
		handle->setHandleGroup(lsPtr);
		handles.push_back(handle);
	}
    
	for(auto& handle : handles)
	{
		handle->start();
	}

	_epollServer->createEpoll();

    //vNetThread[0]->createEpoll(1);

    unsigned int iNetThreadNum = _epollServer->getNetThreadNum();
    vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

	for (size_t i = 0; i < iNetThreadNum; ++i)
    {
        vNetThread[i]->start();
    }

    while(!_epollServer->isTerminate())
    {

	}	

//记得补充这里关闭机制
/*
    if(_epollServer->isTerminate())
    {
        for(size_t i = 0; i < iNetThreadNum; ++i)
        {
            vNetThread[i]->terminate();
            vNetThread[i]->getThreadControl().join();
        }

        _epollServer->stopThread();
    }
*/
    //handle.setEpollServer(_epollServer.get());   

    //handle.start();

    //vNetThread[0]->run();
    return 0;
}
