#include "ServantHandle.h"
#include "ServantHelper.h"

namespace tars
{

ServantHandle::ServantHandle()
{
    
}

ServantHandle::~ServantHandle()
{
    map<string, ServantPtr>::iterator it = _servants.begin();

    while(it != _servants.end())
    {
        try
        {
            it->second->destroy();
        }
        catch(exception &ex)
        {
            //TLOGERROR("[TARS]ServantHandle::destroy error:" << ex.what() << endl);
        }
        catch(...)
        {
            //TLOGERROR("[TARS]ServantHandle::destroy unknown exception error" << endl);
        }
        ++it;
    }

}


void ServantHandle::initialize()
{
    ServantPtr servant = ServantHelperManager::getInstance()->create(_lsPtr->getName());
    _servants[servant->getName()] = servant;

    servant->setHandle(this);
    servant->initialize();

}

}
