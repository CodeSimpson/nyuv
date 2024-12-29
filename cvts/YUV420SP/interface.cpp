#include "YUV420SP.h"

static CVT *create(const STR& type)
{
    return new cvt::YUV420SP(type);
}

static VOID destroy(CVT *p)
{
    if (p)
    {
        delete p;
        p = nullptr;
    }
}

extern "C"
{
    CDK_VISIBILITY_PUBLIC VOID CVTEntry(CALLBACKS &cvtCallBacks)
    {
        cvtCallBacks.create = create;
        cvtCallBacks.destroy = destroy;
    }
}