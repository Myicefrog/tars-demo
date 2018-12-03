#ifndef __TARS_ENDPOINT_INFO_H_
#define __TARS_ENDPOINT_INFO_H_

#include "NetworkUtil.h"
#include "Global.h"

using namespace std;

namespace tars
{
class EndpointInfo
{
public:

	EndpointInfo();

	EndpointInfo(const string& host, uint16_t port);

	string host() const;

	uint16_t port() const;

	const struct sockaddr_in& addr() const;
	
protected:

	string                   _host;
	uint16_t               _port;
	struct sockaddr_in     _addr;
};

}

#endif
