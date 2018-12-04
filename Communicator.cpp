#include "Communicator.h"

namespace tars
{

Communicator::Communicator()
{
	memset(_communicatorEpoll,0,sizeof(_communicatorEpoll));
}

Communicator::~Communicator()
{
    //terminate();
}

ServantProxy * Communicator::getServantProxy(const string& objectName)
{
    Communicator::initialize();

    return _servantProxyFactory->getServantProxy(objectName,setName);
}

void Communicator::initialize()
{
	size_t i = 0;
	catorEpoll[i] = new CommunicatorEpoll(this, i);
    _communicatorEpoll[i]->start();
}

}
