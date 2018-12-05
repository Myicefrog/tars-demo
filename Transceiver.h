#ifndef __TARS_TRANSCEIVER_H_
#define __TARS_TRANSCEIVER_H_

#include <list>
#include "NetworkUtil.h"
#include "CommunicatorEpoll.h"

using namespace std;

namespace tars
{

class ObjectProxy;

class Transceiver
{
public:

    enum ReturnStatus
    {
        eRetError = -1,
        eRetOk    = 0,
        eRetFull  = 1,
    };


	Transceiver(ObjectProxy *objectProxy, const string& ip, const uint16_t& port );

	virtual ~Transceiver();	

	bool isValid() const
    {
        return (_fd != -1);
    }

	virtual void close();

	void connect();

	ObjectProxy * getObjProxy() { return _objectProxy; }

    virtual int fd() const
    {
        return _fd;
    }

	virtual int doRequest();

	int sendRequest(const char * pData,size_t iSize, bool forceSend = false);
	
	virtual int send(const void* buf, uint32_t len, uint32_t flag);


protected:

	int                      _fd;
	
	string _ip;
	uint16_t _port;

	FDInfo                   _fdInfo;	
	
	string 					_sendBuffer;
	
	string 					_recvBuffer;

	ObjectProxy*                           _objectProxy;
};

}

#endif
