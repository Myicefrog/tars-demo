#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServer  _epollServer(1);
	vector<TC_EpollServer::NetThread*> vNetThread = _epollServer.getNetThread();

	vNetThread[0]->run();
	return 0;
}
