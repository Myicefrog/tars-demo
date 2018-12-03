#ifndef __TARS_COMMUNICATOR_H_
#define __TARS_COMMUNICATOR_H_

#include "CommunicatorEpoll.h"
#include "ServantProxy.h"

namespace tars
{

class Communicator
{
public:

	Communicator();

	~Communicator();

    template<class T> void stringToProxy(const string& objectName, T& proxy,const string& setName="")
    {
        ServantProxy * pServantProxy = getServantProxy(objectName,setName);
        proxy = (typename T::element_type*)(pServantProxy);
    }

protected:
	
	ServantProxy * getServantProxy(const string& objectName);

	void initialize();

protected:

	CommunicatorEpoll *    _communicatorEpoll[64];	

};

}

#endif
