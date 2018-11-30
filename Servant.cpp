#include "Servant.h"

namespace tars
{

Servant::Servant():_handle(NULL)
{
}

Servant::~Servant()
{
}

void Servant::setName(const string &name)
{
    _name = name;
}

string Servant::getName() const
{
    return _name;
}

void Servant::setHandle(TC_EpollServer::Handle* handle)
{
    _handle = handle;
}

TC_EpollServer::Handle* Servant::getHandle()
{
    return _handle;
}

}
