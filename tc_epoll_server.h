#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <memory>

#include "tc_epoller.h"
#include "tc_socket.h"
#include "tc_thread.h"
#include "tc_thread_queue.h"
#include "tc_clientsocket.h"


using namespace std;

namespace tars
{

class TC_EpollServer
{
public:

    class NetThread;

    class BindAdapter;    
	typedef shared_ptr<BindAdapter> BindAdapterPtr;

    class Handle;
	typedef shared_ptr<Handle> HandlePtr;

    struct tagRecvData
    {
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
        int64_t         recvTimeStamp;  /**接收到数据的时间*/
        bool            isOverload;     /**是否已过载 */
        bool            isClosed;       /**是否已关闭*/
        int                fd;                /*保存产生该消息的fd，用于回包时选择网络线程*/
       // BindAdapterPtr  adapter;        /**标识哪一个adapter的消息*/
        int             closeType;     /*如果是关闭消息包，则标识关闭类型,0:表示客户端主动关闭；1:服务端主动关闭;2:连接超时服务端主动关闭*/
    };

    struct tagSendData
    {
        char            cmd;            /**命令:'c',关闭fd; 's',有数据需要发送*/
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
    };

    typedef TC_ThreadQueue<tagRecvData*, deque<tagRecvData*> > recv_queue;
    typedef TC_ThreadQueue<tagSendData*, deque<tagSendData*> > send_queue;
    typedef recv_queue::queue_type recv_queue_type;

    class Handle : public TC_Thread, public TC_ThreadLock
    {
    public:

        Handle();

        virtual ~Handle();

        void setEpollServer(TC_EpollServer *pEpollServer);

        TC_EpollServer* getEpollServer();

        virtual void run();

    public:

        void sendResponse(unsigned int uid, const string &sSendBuffer, const string &ip, int port, int fd);

        void close(unsigned int uid, int fd);

//        void setWaitTime(uint32_t iWaitTime);

        virtual void initialize() {};

//        virtual void notifyFilter();

        bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);
		
		friend class BindAdapter;

    protected:

        TC_EpollServer  *_pEpollServer;

        uint32_t  _iWaitTime;


   protected:

        virtual void handleImp();
    };
	
	class BindAdapter : public TC_ThreadLock
	{
	public:
		
		BindAdapter(){}
		
		BindAdapter(TC_EpollServer *pEpollServer);

		~BindAdapter();

        void setEndpoint(const string &str,const int &port);

        TC_Endpoint getEndpoint() const;

        TC_Socket &getSocket();

        TC_EpollServer* getEpollServer();

        void insertRecvQueue(const recv_queue::queue_type &vtRecvData,bool bPushBack = true);

        bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);

        template<typename T> void setHandle()
		{

		}

    protected:
        friend class TC_EpollServer;
        friend class NetThread;

        TC_EpollServer  *_pEpollServer;

        TC_Socket       _s;

        TC_Endpoint     _ep;

        recv_queue      _rbuffer;		
        
        TC_ThreadLock               monitor;
 
	};

	class NetThread
	{
	public:
		NetThread(TC_EpollServer *epollServer);
		virtual ~NetThread();	

		int bind(string& ip, int& port);

        int bind(BindAdapterPtr &lsPtr);

        void bind(const TC_Endpoint &ep, TC_Socket &s);

		void run();		

		void createEpoll(uint32_t iIndex = 0);

		bool accept(int fd);

		void processNet(const epoll_event &ev);

		void processPipe();

		enum
        	{
            		ET_LISTEN = 1,
            		ET_CLOSE  = 2,
            		ET_NOTIFY = 3,
            		ET_NET    = 0,
        	};

		struct
		{
			string response;
			uint32_t uid;	
		}_response;


        void insertRecvQueue(const recv_queue::queue_type &vtRecvData,bool bPushBack = true);

		void send(unsigned int uid, const string &s, const string &ip, uint16_t port);
             
		bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);


	private:
		TC_EpollServer            *_epollServer;

		TC_Socket                   _shutdown;
	 	TC_Socket                   _notify;

		TC_Socket                   _bind_listen;

		TC_Epoller                  _epoller;

		string              _recvbuffer;

		string              response;

		map<int,int>        _listen_connect_id;

		list<uint32_t>                  _free;

		volatile size_t                 _free_size;

        recv_queue      _rbuffer;

	    send_queue                  _sbuffer;

		//map<int, BindAdapter*>    _listeners;

		map<int, BindAdapterPtr>    _listeners;

        
    public:

                TC_ThreadLock               monitor;
	};



public:
	TC_EpollServer(unsigned int iNetThreadNum = 1);
	~TC_EpollServer();

public:
    vector<TC_EpollServer::NetThread*> getNetThread() { return _netThreads; }
    
    void send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd);

    int  bind(TC_EpollServer::BindAdapter* lsPtr);

    int  bind(TC_EpollServer::BindAdapterPtr &lsPtr);

protected:

    friend class BindAdapter;
	
private:
	std::vector<NetThread*>        _netThreads;
    
	vector<HandlePtr>           _handles;
};
typedef shared_ptr<TC_EpollServer> TC_EpollServerPtr;
}

#endif
