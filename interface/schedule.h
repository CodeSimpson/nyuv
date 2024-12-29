/*
 * @Author: your name
 * @Date: 2020-06-30 10:03:09
 * @LastEditTime: 2020-08-27 14:54:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /nyuv/interface/schedule.h
 */
#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "cvtbase.h"

#include <map>
#include <set>
#include <string>

using namespace std;

typedef struct __LIB__
{
    string libname;
    string source;
    string target;
    CVTENTRY instance;

    public:
    __LIB__(const string &a, const string &b, const string &c)
    {
        libname = a;
        source = b;
        target = c;
    }

    bool operator < (const __LIB__ &lib) const
    {
        if (source == lib.target)
            return false;
        else
            return true;
    }
}LIB;


class SCHEDULE
{
public:
    SCHEDULE();
    ~SCHEDULE();

    RESULT process(const string &type, REQUESTINFO &requestInfo);
    STR parseFormat(const string &type, REQUESTINFO &requestInfo);
    std::map<string, float> parseScoreBySize(const FORMAT& format);

private:
    RESULT getLibList();
    RESULT linkTest();
    RESULT loadLibs();
    RESULT initLibs();
    RESULT uninitLibs();
    BOOL success();
    RESULT iterator(const string &type, REQUESTINFO &requestInfo);
    RESULT loadInputFile(REQUESTINFO &requestInfo);

private:

    TYPE m_type;
    string m_target;
    string m_source;
    REQUESTINFO m_inputinfo;
    REQUESTINFO m_outputinfo;

    map<string, void *> m_libinstance;
    map<string, LIB> m_libs;
    map<string, TYPE> m_typemap;
    map<string, std::pair<CALLBACKS, CVT *>> m_cvts;

    RESULT m_init;
};
#endif