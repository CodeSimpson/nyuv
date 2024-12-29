#include "Bayer8.h"

static CVT *create(const STR& type)
{
    return new cvt::Bayer8(type);
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