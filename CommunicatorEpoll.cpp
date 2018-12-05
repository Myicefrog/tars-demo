#include "CommunicatorEpoll.h"
#include "AsyncProcThread.h"
#include "ObjectProxy.h"

using namespace std;

namespace tars
{
CommunicatorEpoll::CommunicatorEpoll(size_t netThreadSeq)
//: _communicator(pCommunicator)
{
    _ep.create(1024);

    _shutdown.createSocket();
    _ep.add(_shutdown.getfd(), 0, EPOLLIN);

    //ObjectProxyFactory 对象
    //_objectProxyFactory = new ObjectProxyFactory(this);

    //异步线程数
    _asyncThreadNum = 3;

    if(_asyncThreadNum == 0)
    {
        _asyncThreadNum = 3;
    }

    //创建异步线程
    for(size_t i = 0; i < _asyncThreadNum; ++i)
    {
        _asyncThread[i] = new AsyncProcThread(10000);
        _asyncThread[i]->start();
    }

    //初始化请求的事件通知
    for(size_t i = 0; i < 2048; ++i)
    {
        _notify[i].bValid = false;
    }
}

CommunicatorEpoll::~CommunicatorEpoll()
{
}

void CommunicatorEpoll::addFd(int fd, FDInfo * info, uint32_t events)
{
    _ep.add(fd,(uint64_t)info,events);
}

void CommunicatorEpoll::delFd(int fd, FDInfo * info, uint32_t events)
{
	cout<<"defFd"<<endl;
    _ep.del(fd,(uint64_t)info,events);
}

void CommunicatorEpoll::notify(size_t iSeq,ReqInfoQueue * msgQueue)
{

    if(_notify[iSeq].bValid)
    {
        _ep.mod(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
        assert(_notify[iSeq].stFDInfo.p == (void*)msgQueue);
    }
    else
    {
        _notify[iSeq].stFDInfo.iType   = FDInfo::ET_C_NOTIFY;
        _notify[iSeq].stFDInfo.p       = (void*)msgQueue;
        _notify[iSeq].stFDInfo.fd      = _notify[iSeq].eventFd;
        _notify[iSeq].stFDInfo.iSeq    = iSeq;
        _notify[iSeq].notify.createSocket();
        _notify[iSeq].bValid           = true;

        _ep.add(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
		cout<<"CommunicatorEpoll::notify _ep.add"<<endl;
    }
}

void CommunicatorEpoll::run()
{
    while (true)
    {
        try
        {
            int num = _ep.wait(100);

            //先处理epoll的网络事件
            for (int i = 0; i < num; ++i)
            {
                const epoll_event& ev = _ep.get(i);
                uint64_t data = ev.data.u64;

                if(data == 0)
                {
                    continue; //data非指针, 退出循环
                }

				cout<<"CommunicatorEpoll handle"<<endl;
				cout<<"ev.events is "<<ev.events<<endl;
				cout<<"EPOLLIN is "<<EPOLLIN<<endl;
				cout<<"EPOLLOUT is "<<EPOLLOUT<<endl;
                handle((FDInfo*)data, ev.events);
            }
        }
        catch (exception& e)
        {
        }
        catch (...)
        {
        }
    }
}


void CommunicatorEpoll::handle(FDInfo * pFDInfo, uint32_t events)
{
    try
    {
        assert(pFDInfo != NULL);

        //队列有消息通知过来
        if(FDInfo::ET_C_NOTIFY == pFDInfo->iType)
        {
            ReqInfoQueue * pInfoQueue=(ReqInfoQueue*)pFDInfo->p;
            ReqMessage * msg = NULL;

            try
            {
                while(pInfoQueue->pop_front(msg))
                {
                    //线程退出了
                    if(ReqMessage::THREAD_EXIT == msg->eType)
                    {
                        assert(pInfoQueue->empty());

                        delete msg;
                        msg = NULL;

                        _ep.del(_notify[pFDInfo->iSeq].notify.getfd(),(long long)&_notify[pFDInfo->iSeq].stFDInfo, EPOLLIN);

                        delete pInfoQueue;
                        pInfoQueue = NULL;

                        _notify[pFDInfo->iSeq].stFDInfo.p = NULL;
                        _notify[pFDInfo->iSeq].notify.close();
                        _notify[pFDInfo->iSeq].bValid = false;

                        return;
                    }
					cout<<"msg->sReqData is "<<msg->sReqData<<endl;
                    msg->pObjectProxy->invoke(msg);
                }
            }
            catch(exception & e)
            {
            }
            catch(...)
            {
            }
        }
        else
        {
			cout<<"FDInfo is "<<pFDInfo->iType<<endl;
			Transceiver *pTransceiver = (Transceiver*)pFDInfo->p;
			cout<<"pTransceiver fd is "<<pTransceiver->fd()<<endl;
            //先收包
			cout<<"events is "<<events<<endl;
			cout<<"events & EPOLLIN is "<<(events & EPOLLIN)<<endl;
			cout<<"events & EPOLLOUT is "<<(events & EPOLLOUT)<<endl;

            if (events & EPOLLIN)
            {
				cout<<"handleInputImp"<<endl;
            	handleInputImp(pTransceiver);
            }

            //发包
            if (events & EPOLLOUT)
            {
				cout<<"handleOutputImp"<<endl;
                handleOutputImp(pTransceiver);
            }

            //连接出错 直接关闭连接
            if(events & EPOLLERR)
            {
            }
        }
    }
    catch(exception & e)
    {
    }
    catch(...)
    {
    }
}


void CommunicatorEpoll::handleInputImp(Transceiver * pTransceiver)
{
}


void CommunicatorEpoll::handleOutputImp(Transceiver * pTransceiver)
{
	pTransceiver->doRequest();

}


}
