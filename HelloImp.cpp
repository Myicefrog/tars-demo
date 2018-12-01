#include "HelloImp.h"

using namespace std;

void HelloImp::initialize()
{
    //initialize servant here:
    //...
	cout<<"HelloImp::initialize"<<endl;
}


void HelloImp::destroy()
{
    //destroy servant here:
    //...
	cout<<"HelloImp::destroy"<<endl;
}

int HelloImp::doRequest(const string& request, vector<char> &buffer)
{
	cout<<"coming request is "<<request<<endl;

	string response = "very good man";
	
	if(request == "hello")
	{
		cout<<"hello request is "<<request<<" size is "<<request.size()<<endl;
		response = "hello response";
	}
	else
	{
		cout<<"no hello request is "<<request<<" size is "<<request.size()<<endl;
		response = "buddy, what are you doing";
	}

	buffer.assign(response.begin(),response.end());

	return 0;
}
