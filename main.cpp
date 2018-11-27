#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{
	//TC_EpollServerPtr _epollServer = new TC_EpollServer(1);

	TC_EpollServerPtr _epollServer = make_shared<TC_EpollServer>(1);

    //TC_EpollServer*  _epollServer = new TC_EpollServer(1);

    vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

    string ip = "127.0.0.1";

    int port = 9877;

    TC_EpollServer::BindAdapterPtr lsPtr = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

    lsPtr->setEndpoint(ip,port);

    _epollServer->bind(lsPtr);

     //vNetThread[0]->bind(ip,port);

    vNetThread[0]->createEpoll(1);
    
    vector<TC_EpollServer::HandlePtr>          handles;

	int handleNum = 4;

	for (int32_t i = 0; i < handleNum; ++i)
	{
		TC_EpollServer::HandlePtr handle = make_shared<TC_EpollServer::Handle>();
		handle->setEpollServer(_epollServer.get());
		handles.push_back(handle);
	}
    
	for(auto& handle : handles)
	{
		handle->start();
	}
	//TC_EpollServer::Handle handle;

    //handle.setEpollServer(_epollServer.get());   

    //handle.start();

    vNetThread[0]->run();
    return 0;
}
