#include "log.h"
#include "threadpool.h"

ThreadPool *Thread::p_thread_instance = nullptr;
using namespace Thread;
class ThreadGarbo
{
public:
    ~ThreadGarbo()
    {
        if (p_thread_instance)
        {
            delete p_thread_instance;
            p_thread_instance = nullptr;
        }
    }
};
static ThreadGarbo thread_garbo;
