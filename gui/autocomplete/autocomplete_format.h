#ifndef __AUTOFORMAT_H__
#define __AUTOFORMAT_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

#include "defines.h"

using namespace std;

enum class SOURCETYPE
{
    INVALID,
    HEIGHT = 0,
    WIDTH,
    STRIDE,
};

class AutoCompleteFormat
{
public:
    static AutoCompleteFormat *getInstance();
    ~AutoCompleteFormat();

    RESULT autocomplete(IMAGEINFO &predict, IMAGEINFO &src, const SOURCETYPE);
    RESULT add(IMAGEINFO &src);
    RESULT initInfo(IMAGEINFO &predict);
    BOOL check(int &h, int &w);

private:
    class Garbo
    {
    public:
        ~Garbo()
        {
            if (p_autocomplete_instance)
            {
                delete p_autocomplete_instance;
            }
        }
    };
    static Garbo garbo;
    static AutoCompleteFormat *p_autocomplete_instance;

    AutoCompleteFormat();
    typedef tuple<string, string, string, int> CONFSTRUCT;
    enum CONFSTRUCTINDEX : size_t
    {
        HEIGHT = 0,
        WIDTH,
        STRIDE,
        WEIGHT,
    };

    string m_config_path;
    vector<CONFSTRUCT> m_originlist;
    vector<int> m_candidate;

private:
    RESULT initPath();
    RESULT load();
    RESULT dump();
    RESULT parse(CONFSTRUCT &src, const SOURCETYPE);
    RESULT update(IMAGEINFO &predict);
};

#endif