#ifndef __TC_THREAD_QUEUE_H_
#define __TC_THREAD_QUEUE_H_

#include <deque>
#include <vector>
#include <cassert>
#include "tc_monitor.h"
#include "tc_lock.h"

namespace tars
{

template<typename T, typename D = deque<T> >
class TC_ThreadQueue : protected TC_ThreadLock
{
public:
    TC_ThreadQueue():_size(0){};

public:

    typedef D queue_type;

    bool pop_front(T& t, size_t millsecond = 0);

    void notifyT();

    void push_back(const T& t);

    void push_back(const queue_type &qt);

    void push_front(const T& t);

    void push_front(const queue_type &qt);

    bool swap(queue_type &q, size_t millsecond = 0);

    size_t size() const;

    void clear();

    bool empty() const;

protected:

    queue_type          _queue;

    size_t              _size;

};

}

#endif
