#ifndef __TARS_COMMUNICATOR_H_
#define __TARS_COMMUNICATOR_H_

#include "CommunicatorEpoll.h"

namespace tars
{

class Communicator
{
public:

	Communicator();

	~Communicator();

protected:

	CommunicatorEpoll *    _communicatorEpoll[64];	

};

}

#endif
