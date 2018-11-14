#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServer  _epollServer(1);
	vector<TC_EpollServer::NetThread*> vNetThread = _epollServer.getNetThread();
	string ip = "192.168.128.139";
	int port = 9877;
	vNetThread[0]->bind(ip,port);
	vNetThread[0]->run();
	return 0;
}
