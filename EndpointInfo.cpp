#include "EndpointInfo.h"

namespace tars
{
EndpointInfo::EndpointInfo()
: _port(0)
{
    memset(&_addr,0,sizeof(struct sockaddr_in));
}

EndpointInfo::EndpointInfo(const string& host, uint16_t port)
: _host(host)
, _port(port)
{
    try
    {
        NetworkUtil::getAddress(_host, _port, _addr);
    }
    catch (...)
    {
		cout<<"EndpointInfo exception"<<endl;
    }
}

string EndpointInfo::host() const
{
    return string(_host);
}

string EndpointInfo::host() const
{
    return string(_host);
}

uint16_t EndpointInfo::port() const
{
    return _port;
}

const struct sockaddr_in& EndpointInfo::addr() const
{
    return _addr;
}

}
