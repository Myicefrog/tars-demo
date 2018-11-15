#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServer  _epollServer(1);
	vector<TC_EpollServer::NetThread*> vNetThread = _epollServer.getNetThread();
	string ip = "127.0.0.1";
	int port = 9877;
	vNetThread[0]->bind(ip,port);
	vNetThread[0]->createEpoll(1);
	vNetThread[0]->run();
	return 0;
}
