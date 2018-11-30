#ifndef __TARS_SERVANT_H_
#define __TARS_SERVANT_H_

#include "tc_epoll_server.h"

namespace tars
{
	
class Servant
{
public:

	Servant();

	~Servant();

	void setHandle(TC_EpollServer::Handle* handle);

	TC_EpollServer::Handle* getHandle();

	void setName(const string &name);

	string getName() const;

	virtual void initialize() = 0;

	virtual void destroy() = 0;

protected:

	TC_EpollServer::Handle* _handle;
};
	
typedef shared_prt<Servant> ServantPtr;

}

#endif
