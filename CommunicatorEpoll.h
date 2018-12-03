#ifndef __TARS__COMMUNICATOR_EPOLL_H_
#define __TARS__COMMUNICATOR_EPOLL_H_

#include "tc_thread.h"
#include "tc_thread_mutex.h"
#include "tc_epoller.h"
//#include "tc_loop_queue.h"
//#include "Message.h"
#include <set>

namespace tars
{

struct FDInfo
{
    enum
    {
        ET_C_NOTIFY = 1,
        ET_C_NET    = 2,
    };

    /**
     * 构造函数
     */
    FDInfo()
    : iSeq(0)
    , fd(-1)
    , iType(ET_C_NOTIFY)
    , p(NULL)
    {
    }

    /**
     * 析构函数
     */
    ~FDInfo()
    {
    }

    size_t iSeq;

    int    fd;

    int    iType;

    void * p;
};

class Communicator;


class CommunicatorEpoll : public TC_Thread
{
public:
	
	CommunicatorEpoll(Communicator * pCommunicator, size_t _netThreadSeq);

	virtual ~CommunicatorEpoll();

    inline Communicator * getCommunicator()
    {
        return _communicator;
    }


	virtual void run();

	void addFd(int fd,FDInfo * info, uint32_t events);

	void delFd(int fd,FDInfo * info, uint32_t events);

protected:

	void handle(FDInfo * pFDInfo, uint32_t events);

	void handleInputImp(Transceiver * pTransceiver);

	void handleOutputImp(Transceiver * pTransceiver);
	
protected:

	Communicator *         _communicator;

	TC_Socket              _shutdown;

	TC_Epoller             _ep;
};


}


#endif
