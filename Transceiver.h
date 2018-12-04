#ifndef __TARS_TRANSCEIVER_H_
#define __TARS_TRANSCEIVER_H_

#include <list>
#include "NetworkUtil.h"
#include "CommunicatorEpoll.h"

using namespace std;

namespace tars
{
class AdapterProxy;

class Transceiver
{
public:

	Transceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep);

	virtual ~Transceiver();	

	bool isValid() const
    {
        return (_fd != -1);
    }

	virtual void close();


protected:

	int                      _fd;

	FDInfo                   _fdInfo;	
	
	AdapterProxy *           _adapterProxy;

	string 					_sendBuffer;
	
	string 					_recvBuffer

};

}

#endif
