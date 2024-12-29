#include "threadpool.h"

namespace Thread
{
    extern ThreadPool *p_thread_instance;
    ThreadPool *getThreadInstance()
    {
        if (!p_thread_instance)
            p_thread_instance = new ThreadPool(ThreadPool::max_cpu());
        return p_thread_instance;
    }
}; // namespace Thread