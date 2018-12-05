#include <iostream>
#include <string>
#include "CommunicatorEpoll.h"

using namespace std;
using namespace tars;

int main()
{

	CommunicatorEpoll* _communicatorEpoll =  new CommunicatorEpoll(0);
	
	_communicatorEpoll->start();


	while(true)
	{
	
	}

	return 0;
}

