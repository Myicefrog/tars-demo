#include <iostream>
#include <string>
#include "CommunicatorEpoll.h"
#include "ObjectProxy.h"

using namespace std;
using namespace tars;

int main()
{

	CommunicatorEpoll* _communicatorEpoll =  new CommunicatorEpoll(0);
	
	_communicatorEpoll->start();

//////////////////////////////////////////////////////////////////////
	
	ReqMessage * msg = new ReqMessage();

	msg->init(ReqMessage::ASYNC_CALL);

	//msg->request = "hello";
	msg->request = "hello,world";


	string host = "127.0.0.1";
    uint16_t port = 9877;

	ObjectProxy * pObjectProxy = new ObjectProxy(_communicatorEpoll, host, port);

	msg->pObjectProxy = pObjectProxy;

	struct timeval tv;
	::gettimeofday(&tv, NULL);
    msg->iBeginTime = tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;


	if(msg->eType == ReqMessage::SYNC_CALL)
	{
		msg->bMonitorFin = false;

		msg->pMonitor = new ReqMonitor;
	}

	ReqInfoQueue * pReqQ    = new ReqInfoQueue(1000);

	bool bEmpty = false;	
	
	bool bSync  = (msg->eType == ReqMessage::SYNC_CALL);

	pReqQ->push_back(msg,bEmpty);

	_communicatorEpoll->notify(0, pReqQ);

	if(bSync)
	{

		if(!msg->bMonitorFin)
    	{
    		TC_ThreadLock::Lock lock(*(msg->pMonitor));

        	//等待直到网络线程通知过来
        	if(!msg->bMonitorFin)
        	{
        		msg->pMonitor->wait();
        	}
		}
		
		cout<<"msg->response is "<<msg->response<<endl;
	
		return 0;
	}
}
