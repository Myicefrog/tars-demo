#include "CommunicatorEpoll.h"

using namespace std;

namespace tars
{
CommunicatorEpoll::CommunicatorEpoll(size_t netThreadSeq)
: _communicator(pCommunicator)
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
        _asyncThread[i] = new AsyncProcThread(iAsyncQueueCap);
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
    _ep.del(fd,(uint64_t)info,events);
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

                    //msg->pObjectProxy->invoke(msg);
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
            //先收包
            if (events & EPOLLIN)
            {
            	handleInputImp();
            }

            //发包
            if (events & EPOLLOUT)
            {
                handleOutputImp();
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


void CommunicatorEpoll::handleInputImp()
{
}


void CommunicatorEpoll::handleOutputImp()
{
}


}
