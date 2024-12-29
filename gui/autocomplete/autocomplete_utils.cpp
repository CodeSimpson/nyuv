#include "autocomplete_utils.h"
#include "autocomplete_format.h"
#include "cvtinterface.h"
#include "defines.h"

#undef __LOGTAG__
#define __LOGTAG__ "AC_UTILS"
#include "log.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <map>

using namespace std;

VOID split(string &s, const char delim, vector<string> *ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != string::npos)
    {
        ret->push_back(s.substr(last, index - last));
        last = index;
        for (int i = index; s[i] == delim; i++)
        {
            last++;
        }
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0)
    {
        ret->push_back(s.substr(last, index - last));
    }
}

float string_same(string &str1, string &str2)
{
    size_t len1 = str1.size();
    size_t len2 = str2.size();

    if (len1 < 1 || len2 < 1)
        return 0;

    string strm = str1, strn = str2;
    size_t lenm = len1, lenn = len2;
    if (len1 < len2)
    {
        strm = str2;
        strn = str1;
        lenm = len2;
        lenn = len1;
    }

    size_t counts = 0;
    vector<string> cands;

    for (size_t i = 0; i < (lenm - lenn); i++)
    {
        string cand = "";
        for (size_t j = i; j < lenn; j++)
        {
            cand += strm[j];
        }
        cands.push_back(cand);
    }
    for (string &str : cands)
    {
        size_t counts_tmp = 0;
        for (size_t i = 0; i < lenn; i++)
        {
            if (str[i] == strn[i])
            {
                counts_tmp++;
            }
            else
            {
                break;
            }
        }
        if (counts_tmp > counts)
        {
            counts = counts_tmp;
        }
    }

    return (float)counts / (float)lenn;
}

//<origin, upper >
static vector<pair<string, string>> upper_types; //每次软件启动，每一份都一样
RESULT parse_type(IMAGEINFO &out, FILEINFO &file, vector<string> &types)
{
    //TODO: 先不要删除, 后续还要看看这两个方法是不是需要结合起来
    //先都转化为大写, 如果static容器为空，则先初始化
    // if (upper_types.empty())
    // {
    //     for (auto t : types)
    //     {
    //         string type = t;
    //         //统一大小写（大写）
    //         //类型中的字符排序, 把数字排到前面;
    //         transform(type.begin(), type.end(), type.begin(), ::toupper);
    //         sort(type.begin(), type.end());
    //         upper_types.emplace_back(pair<string, string>(t, type));
    //     }
    // }

    // string type = file.suffix;
    // transform(type.begin(), type.end(), type.begin(), ::toupper);
    // sort(type.begin(), type.end());
    // int scoreMax = 0;
    // for (auto t : upper_types)
    // {
    //     //至少要有两个匹配,
    //     //按字符匹配
    //     int score = -2;
    //     for_each(type.begin(), type.end(), [=, &score](char &c) {
    //         if (string::npos != t.second.find(c))
    //         {
    //             score++;
    //         }
    //     });

    //     if (scoreMax < score)
    //     {
    //         scoreMax = score;
    //         out.type = t.first;
    //     }
    // }

    //如果通过后缀名看不出格式
    // if (scoreMax <= 0)
    // {
    FORMAT format = out.format;
    format.size = file.size;
    SCORE score = cvt_parseScoreBySize(format);
    FLOAT score_max = .0;
    for (auto &it : score)
    {
        if (score_max < it.second)
        {
            score_max = it.second;
            out.type = it.first;
        }
    }
    // }

    float same_score = 0;
    for (auto &it : types)
    {
        string suffix = file.suffix;
        transform(suffix.begin(), suffix.end(), suffix.begin(), ::toupper);
        string type = it;
        transform(type.begin(), type.end(), type.begin(), ::toupper);
        float same_score_tmp = string_same(suffix, type);
        if ((same_score_tmp > 0.74) && (same_score_tmp > same_score))
        {
            same_score = same_score_tmp;
            out.type = it;
            LOGI("suffix score {}", same_score);
        }
    }

    return RESULT::SUCCESS;
}

RESULT parse_filename(IMAGEINFO &out, FILEINFO &file, vector<string> &types)
{
    string filename = file.filename;
    RESULT ret = RESULT::SUCCESS;
    vector<INT32> number_lists;

    string number;
    for (auto c : filename)
    {
        //连续数字存下来, 是被选的size
        if ((c >= '0') && (c <= '9'))
        {
            number += c;
        }
        else
        {
            //存了一组连续数字, 把太大的去掉, 因为图片不会很大
            if (!number.empty())
            {
                if (number.size() <= 6)
                    number_lists.emplace_back(stoi(number, nullptr));
            }
            number.clear();
        }
    }

    //配对，例如
    //2020， 0， 0， 11， 11， 1920， 1080
    //生成
    //(11, 11) (11, 1920) (1920, 1080)
    vector<pair<INT32, INT32>> pairs;
    pair<INT32, INT32> pair_tmp(0, 0);
    for (auto num : number_lists)
    {
        if ((num != 0) && (pair_tmp.first == 0))
        {
            pair_tmp.first = num;
        }
        else if ((num != 0) && (pair_tmp.first != 0) && (pair_tmp.second == 0))
        {
            pair_tmp.second = num;
        }
        else
        {
            pair_tmp.first = 0;
            pair_tmp.second = 0;
        }

        if ((pair_tmp.first != 0) && (pair_tmp.second != 0))
        {
            pairs.emplace_back(pair_tmp);
            pair_tmp.first = pair_tmp.second;
            pair_tmp.second = 0;
        }
    }

    bool sizeOK = false;
    for (auto p : pairs)
    {
        //先检查配置表里面是否有这个size
        BOOL ispasshw = AutoCompleteFormat::getInstance()->check(p.first, p.second);
        BOOL ispasswh = AutoCompleteFormat::getInstance()->check(p.second, p.first);

        if (ispasshw || ispasswh)
        {
            out.format.height = ispasshw ? p.first : p.second;
            out.format.width = ispasshw ? p.second : p.first;
            sizeOK = true;
            break;
        }
    }
    if (!sizeOK)
    {
        float disSize = file.size;
        for (auto p : pairs)
        {
            float ratio = float(min(p.first, p.second)) / float(max(p.first, p.second));
            float scale = p.first * p.second;
            //长宽比在0.5-1
            //长宽都一定是偶数（32或64被数？）
            //文件大小是size的x.5或x倍
            //MIPI10    5/4 = 1.25
            //YUV420    3/2 = 1.5
            //RGB       x
            if ((0.5 <= ratio) && (1 >= ratio) && (p.first % 2 == 0) && (p.second % 2 == 0))
            // && (int((file.size / scale) * 4) == ((file.size / scale) * 4)))
            {
                //找到距离最近的一个？可能有点问题
                if (disSize > abs(scale - file.size))
                {
                    disSize = abs(scale - file.size);

                    out.format.height = min(p.first, p.second);
                    out.format.width = max(p.first, p.second);
                    sizeOK = true;
                }
            }
        }
    }
    if (sizeOK)
    {
        out.format.stride = file.size / out.format.height;
    }

    ret = parse_type(out, file, types);

    return ret;
}