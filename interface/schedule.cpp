#include <iostream>
#include <string>
#include <fstream>
#include <dlfcn.h>
#include <utility>

#include "tinyxml2.h"
#include "schedule.h"
#include "tools.h"

#undef __LOGTAG__
#define __LOGTAG__ "SCHEDULE"
#include "log.h"

using namespace std;
using namespace tinyxml2;

SCHEDULE::SCHEDULE()
{
    m_init = RESULT::SUCCESS;
    m_init = getLibList();
    m_init = linkTest();
    m_init = loadLibs();
    m_init = initLibs();
    LOGI("============ INIT DONE ============\n");
}

SCHEDULE::~SCHEDULE()
{
    uninitLibs();
    /***
 * 析构的时候释放dlopen的句柄
 * **/
    for (auto lib = m_libinstance.begin(); lib != m_libinstance.end(); ++lib)
    {
        if (lib->second)
        {
            dlclose(lib->second);
        }
    }

    LOGI("All {} libs had been dlclosed.\n", m_libinstance.size());
    LOGI("============ EXIT ============");
}

BOOL SCHEDULE::success()
{
    return (RESULT::SUCCESS == m_init);
}

/***
 * 解析libxml文件
 * **/
RESULT SCHEDULE::getLibList()
{
    RESULT ret = RESULT::SUCCESS;

    //上一步正常的时候才继续往下走，否则放回RESULT
    //这里的上一步是m_init初始化
    if (!success())
    {
        ret = m_init;
        return ret;
    }

    XMLDocument doc;
    doc.LoadFile(LIBLIST);

    //开始解析xml
    XMLElement *nodelist = doc.RootElement();

    m_target = nodelist->FirstChildElement("SINKTARGET")->GetText();

    XMLElement *node = nodelist->FirstChildElement("NODE");
    while (node)
    {
        string libname = node->FirstChildElement("lib")->GetText();
        libname = "lib" + libname + ".so";
        string source = node->FirstChildElement("source")->GetText();
        string target = node->FirstChildElement("target")->GetText();

        //LIB是自定义的结构体，push到set等容器时需要重写<操作
        LIB lib(libname, source, target);
        m_libs.emplace(source, lib);

        node = node->NextSiblingElement();
    }

    return ret;
}

/***
 * 判断xml中的link关系是否成立
 * **/
RESULT SCHEDULE::linkTest()
{
    RESULT ret = RESULT::SUCCESS;

    //上一步正常的时候才继续往下走，否则放回RESULT
    //主要是因为，一定要正确读取到lib.xml后才能执行后面的步骤，如果读不到也只能跳过，不抛出异常
    if (!success())
    {
        ret = m_init;
        return ret;
    }

    LOGI("checking link...");
    for (auto lib = m_libs.begin(); lib != m_libs.end(); ++lib)
    {
        string type = lib->second.target;

        //target没有下一个节点了，所以不做判断
        if (type == m_target)
            continue;
        decltype(m_libs)::iterator it = m_libs.find(type);
        if (it == m_libs.end()) //需要判断有没有这个key(图像格式)
        {
            LOGE("mismatching link: {} -> {}", lib->second.source, type);
            ret = RESULT::INVALID_TYPE;
        }
    }
    if (ret == RESULT::SUCCESS)
        LOGI("checking link success");
    else
        LOGE("checking link failed");

    return ret;
}

/***
 * 把各个cvt方法存起来，供process调用
 * **/
RESULT SCHEDULE::loadLibs()
{
    RESULT ret = RESULT::SUCCESS;

    //上一步正常的时候才继续往下走，否则放回RESULT
    //主要是因为，一定要正确读取到lib.xml后才能执行后面的步骤，如果读不到也只能跳过，不抛出异常
    if (!success())
    {
        ret = m_init;
        return ret;
    }

    for (auto lib = m_libs.begin(); lib != m_libs.end(); ++lib)
    {
        string libname = lib->second.libname;
        string libkey = lib->second.source;

        //不要重复load lib，所以需要判断是否已经load过了
        if ((m_libinstance.find(libname)) != m_libinstance.end())
            continue;

        string libpath = LIBROOT + libname;

        void *lib_instance = dlopen(libpath.c_str(), RTLD_LAZY);
        if (!lib_instance)
        {
            LOGE("Cannot load library {} -> {} :: {}", libkey, libname, lib_instance);
            LOGE("{}", dlerror());
            ret = RESULT::INVALID_LIB;
            break; //如果有哪个lib读不到，就没有必要往下读了，否则执行的时候可能异常
        }

        //保存lib句柄，程序结束的时候，需要销毁
        m_libinstance.emplace(libname, lib_instance);
    }
    if (ret == RESULT::SUCCESS)
        LOGI("All {} libs had been loaded.", m_libinstance.size());

    for (auto lib = m_libs.begin(); lib != m_libs.end(); ++lib)
    {
        string libname = lib->second.libname;
        auto it = m_libinstance.find(libname);

        if (it == m_libinstance.end())
        {
            ret = RESULT::INVALID_LIB;
            break;
        }

        void *lib_instance = it->second;
        //cvt的对外接口在这里获取，接口必须是C的
        CVTENTRY cvt_entry = (CVTENTRY)dlsym(lib_instance, "CVTEntry");
        const char *dlsym_error = dlerror();
        if (dlsym_error)
        {
            LOGE("Cannot load symbol create {}", dlsym_error);
            LOGE("{}", dlerror());
            ret = RESULT::INVALID_LIB;
            break;
        }

        //把cvt存其来（实际上是创建和销毁函数）
        //link 关系就是从这里体现的，通过map的key找到对应的cvt，所以key一定要正确，key就是图像格式，在xml里面体现
        lib->second.instance = cvt_entry;
    }

    return ret;
}

RESULT SCHEDULE::initLibs()
{
    RESULT ret = RESULT::SUCCESS;

    //上一步正常的时候才继续往下走，否则放回RESULT
    //主要是因为，一定要正确读取到lib.xml后才能执行后面的步骤，如果读不到也只能跳过，不抛出异常
    if (!success())
    {
        ret = m_init;
        return ret;
    }

    for (auto lib = m_libs.begin(); lib != m_libs.end(); ++lib)
    {
        string type = lib->second.source;

        decltype(m_libs)::iterator it = m_libs.find(type);
        CVTENTRY cvt_entry = it->second.instance;
        CALLBACKS cvt_callbacks;
        cvt_entry(cvt_callbacks);
        CVT *cvt = cvt_callbacks.create(type);

        m_cvts.emplace(type, std::pair<CALLBACKS, CVT *>(cvt_callbacks, cvt));
    }

    return ret;
}

RESULT SCHEDULE::uninitLibs()
{
    for (auto lib = m_cvts.begin(); lib != m_cvts.end(); ++lib)
    {
        CALLBACKS cvt_callbacks = lib->second.first;
        CVT *cvt = lib->second.second;

        auto destroy = [=, &cvt_callbacks, &cvt]() -> RESULT {
            (cvt_callbacks.destroy)(cvt);
            return RESULT::SUCCESS;
        };
        destroy();

        // double cost = .0;
        // std::tie(std::ignore, cost) = tools::runByTime<RESULT>(destroy);
        // LOGI("{} destroy cost {}ms", lib->first, cost);
    }

    return RESULT::SUCCESS;
}

std::map<string, float> SCHEDULE::parseScoreBySize(const FORMAT& format)
{
    std::map<string, float> result;
    RESULT ret = RESULT::SUCCESS;
    for (auto &it : m_cvts)
    {
        CVT *cvt = it.second.second;
        STR type = it.first;
        auto processByIter = [&]() mutable -> FLOAT {
            return cvt->parseScoreBySize(format);
        };
        float score = .0;
        double cost = .0;
        std::tie(score, cost) = tools::runByTime<FLOAT>(processByIter);
        result.emplace(type, score);
    }

    return result;
}

STR SCHEDULE::parseFormat(const string &type, REQUESTINFO &requestInfo)
{
    if (!requestInfo.iformat.isValid() || !strcmp(requestInfo.ifilename, ""))
    {
        return "";
    }

    RESULT ret = RESULT::SUCCESS;
    STR format = "";

    if (!success())
    {
        ret = m_init;
        return "";
    }
    m_source = type;

    decltype(m_libs)::iterator it = m_libs.find(type);
    if (it == m_libs.end()) //需要判断有没有这个key(图像格式)
    {
        LOGE("invalid type: {}", type);
        ret = RESULT::INVALID_LIB;
    }
    else
    {
        CVT *cvt = m_cvts.find(type)->second.second;
        if (cvt->supportParseFormat())
        {
            auto processByIter = [&]() mutable -> RESULT {
                ret = loadInputFile(requestInfo);
                if (ret == RESULT::SUCCESS)
                {
                    //cvt node需要设置自己会输出多大的buffer
                    format = cvt->parseFormat(requestInfo);
                }
                return ret;
            };
            double cost = .0;
            std::tie(ret, cost) = tools::runByTime<RESULT>(processByIter);
            LOGI("parse format {} cost {}ms", format, cost);
        }
        else
        {
            LOGI("unsupported hard parse for {}", type);
        }
    }

    return format;
}

RESULT SCHEDULE::process(const string &type, REQUESTINFO &requestInfo)
{
    RESULT ret = RESULT::SUCCESS;

    if (!success())
    {
        ret = m_init;
        return ret;
    }
    m_source = type;

    decltype(m_libs)::iterator it = m_libs.find(type);
    if (it == m_libs.end()) //需要判断有没有这个key(图像格式)
    {
        LOGE("invalid type: {}", type);
        ret = RESULT::INVALID_LIB;
    }
    else
    {
        auto processByIter = [&]() mutable -> RESULT {
            ret = loadInputFile(requestInfo);
            if (ret == RESULT::SUCCESS)
            {
                ret = iterator(type, requestInfo);
            }
            return ret;
        };

        LOGI(">>>>>>>>>> start process {}", type);

        double cost = .0;
        std::tie(ret, cost) = tools::runByTime<RESULT>(processByIter);

        LOGI("<<<<<<<<<< end process {}[{}] cost all {}ms", type, ret, cost);
    }

    return ret;
}

RESULT SCHEDULE::iterator(const string &type, REQUESTINFO &requestInfo)
{
    RESULT ret = RESULT::SUCCESS;

    decltype(m_libs)::iterator it = m_libs.find(type);
    if (it == m_libs.end()) //需要判断有没有这个key(图像格式)
    {
        LOGE("invalid type: {}", type);
        ret = RESULT::INVALID_LIB;
    }
    else
    {
        if (ret == RESULT::SUCCESS)
        {
            CVT *cvt = m_cvts.find(type)->second.second;
            //cvt node需要设置自己会输出多大的buffer
            cvt->setOBufferInfo(requestInfo.oformat, requestInfo.iformat);
            //buffer 在外面设置，所以cvt node无需关系创建buffer
            requestInfo.obuffer.resize(requestInfo.oformat.size);

            double cost = .0;
            std::tie(ret, cost) = tools::runByTime<RESULT>(std::bind(&CVT::cvt, cvt, std::ref(requestInfo)));
            LOGI("processing {} -> {} cost {}ms", type, it->second.target, cost);

            if (ret != RESULT::SUCCESS)
            {
                LOGE("cvt error, return result {}", ret);
                return ret;
            }

            //查看经过cvt转换后的图像格式时候符合要求，
            //如果不符合要求，则通过输出格式找到下一个cvt，直到最终的格式
            if (it->second.target != m_target)
            {
                //如果不是sink目标，则需要交换requet中的input和output信息
                requestInfo.swap();
                //if not the final target type
                string newtype = it->second.target;
                ret = iterator(newtype, requestInfo);
            }
        }
    }

    return ret;
}

RESULT SCHEDULE::loadInputFile(REQUESTINFO &requestInfo)
{
    RESULT ret = RESULT::SUCCESS;
    ifstream inputfile;
    inputfile.open(requestInfo.ifilename, ios::in | ios::binary);
    if (!inputfile)
    {
        LOGE("can not open {}", requestInfo.ifilename);
        return RESULT::INVALID_FILE;
    }

    CVT *cvt = m_cvts.find(m_source)->second.second;
    if (!cvt->testIBuffer(requestInfo.iformat))
    {
        return RESULT::INVALID_FORMAT;
    }
    cvt->getIBufferInfo(requestInfo.iformat);

    inputfile.seekg(0, inputfile.end);
    size_t file_size = inputfile.tellg();
    requestInfo.ibuffer.resize(max(file_size, requestInfo.iformat.size));
    inputfile.seekg(0, inputfile.beg);
    inputfile.read((CHAR *)requestInfo.ibuffer.begin(), requestInfo.ibuffer.size());
    inputfile.close();
    LOGI("source {}x{}, stride {}, size {}", requestInfo.iformat.width, requestInfo.iformat.height, requestInfo.iformat.stride, requestInfo.ibuffer.size());
    requestInfo.ifilename = NULL;

    // if ((requestInfo.iformat.height * requestInfo.iformat.stride) > requestInfo.ibuffer.size())
    // {
    //     return RESULT::INVALID_FORMAT;
    // }

    return ret;
}