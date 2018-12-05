#ifndef __TARS_OBJECT_PROXY_H_
#define __TARS_OBJECT_PROXY_H_

#include "Message.h"
#include "CommunicatorEpoll.h"
#include "Transceiver.h"

namespace tars
{
class ObjectProxy
{
public:
	
	ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll);

	~ObjectProxy();

	void invoke(ReqMessage* msg);

	CommunicatorEpoll * getCommunicatorEpoll()
    {
        return _communicatorEpoll;
    }

protected:
	
	int _fd;

	CommunicatorEpoll *                   _communicatorEpoll;

	std::unique_ptr<Transceiver>           _trans;
};

}

#endif
