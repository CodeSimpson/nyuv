#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "threadpool/ThreadPool.h"

namespace Thread
{
    extern ThreadPool *p_thread_instance;
    ThreadPool *getThreadInstance();
}; // namespace Thread

#endif