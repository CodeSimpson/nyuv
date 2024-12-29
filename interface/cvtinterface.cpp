#include "defines.h"
#include "schedule.h"
#include "cvtinterface.h"

//启动时完成初始化
static SCHEDULE *p_cvt_instance = nullptr;

void cvt_schedul_init()
{
    if (!p_cvt_instance)
        p_cvt_instance = new SCHEDULE();
}

void cvt_schedul_uninit()
{
    if (p_cvt_instance)
    {
        delete p_cvt_instance;
        p_cvt_instance = nullptr;
    }
}

// namespace CVTINTERFACE_PROTECT
// {
//     //回收SCHEDULE垃圾
//     class Garbo
//     {
//     public:
//         ~Garbo()
//         {
//             cvt_schedul_uninit();
//         }
//     };
//     static Garbo garbo;
// } // namespace CVTINTERFACE_PROTECT

RESULT cvt_process(const std::string &type, REQUESTINFO &requestInfo)
{
    RESULT ret = RESULT::SUCCESS;
    if (p_cvt_instance)
        ret = p_cvt_instance->process(type, requestInfo);
    return ret;
}

STR cvt_parseFormat(const std::string &type, REQUESTINFO &requestInfo)
{
    STR ret = "";
    if (p_cvt_instance)
        ret = p_cvt_instance->parseFormat(type, requestInfo);
    return ret;
}

SCORE cvt_parseScoreBySize(const FORMAT& format)
{
    SCORE score;
    if (p_cvt_instance)
        score = p_cvt_instance->parseScoreBySize(format);
    return score;
}