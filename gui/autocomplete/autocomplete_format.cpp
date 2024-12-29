#include "defines.h"
#include "autocomplete_format.h"
#include "autocomplete_utils.h"
#include "autocomplete_config.h"

#include <iostream>

AutoCompleteFormat::Garbo AutoCompleteFormat::garbo;
AutoCompleteFormat *AutoCompleteFormat::p_autocomplete_instance = NULL;
//初始化顺序，调用时候才初始化
AutoCompleteFormat *AutoCompleteFormat::getInstance()
{
    if (p_autocomplete_instance == NULL)
    {
        p_autocomplete_instance = new AutoCompleteFormat();
    }
    return p_autocomplete_instance;
}

AutoCompleteFormat::AutoCompleteFormat()
{
    initPath();
    load();
}
AutoCompleteFormat::~AutoCompleteFormat()
{
    // dump();
}

RESULT AutoCompleteFormat::dump()
{
    vector<string> dump_str;
    for (auto line : m_originlist)
    {
        string dump_line;
        string height_ori, width_ori, stride_ori;
        int weight_ori;
        std::tie(height_ori, width_ori, stride_ori, weight_ori) = line;
        dump_line = height_ori + " " + width_ori + " " + stride_ori + " " +  to_string(weight_ori);
        dump_str.emplace_back(dump_line);
    }
    Config::getInstance()->dump(dump_str, CONFIG_TYPE::FORMAT);

    return RESULT::SUCCESS;
}

RESULT AutoCompleteFormat::initPath()
{
    char const *home = CONFIG;
    m_config_path = string(home) + ".nyuv.config";
}

RESULT AutoCompleteFormat::load()
{
    vector<string> formats = Config::getInstance()->load(CONFIG_TYPE::FORMAT);
    for (auto line : formats)
    {
        CONFSTRUCT format;
        vector<string> datalist;
        split(line, ' ', &datalist);
        m_originlist.emplace_back(CONFSTRUCT(datalist[0],
                                             datalist[1],
                                             datalist[2],
                                             std::stoi(datalist[3], nullptr)));
    }

    return RESULT::SUCCESS;
}

RESULT AutoCompleteFormat::parse(CONFSTRUCT &src, const SOURCETYPE type)
{
    RESULT ret = RESULT::SUCCESS;

    string height_src, width_src, stride_src;
    int weight_src;
    std::tie(height_src, width_src, stride_src, weight_src) = src;

    size_t candidate = 0;
    for (auto line : m_originlist)
    {
        string height_ori, width_ori, stride_ori;
        int weight_ori;
        std::tie(height_ori, width_ori, stride_ori, weight_ori) = line;

        switch (type)
        {
        default:
        case SOURCETYPE::HEIGHT:
            if (height_ori.find(height_src, 0) == 0)
            {
                m_candidate.emplace_back(candidate);
            }
            break;
        case SOURCETYPE::WIDTH:
            if (width_ori.find(width_src, 0) == 0)
            {
                m_candidate.emplace_back(candidate);
            }
            break;
        case SOURCETYPE::STRIDE:
            if (stride_ori.find(stride_src, 0) == 0)
            {
                m_candidate.emplace_back(candidate);
            }
            break;
        }

        candidate++;
    }

    if (m_candidate.size() <= 0)
    {
        ret = RESULT::INVALID_FORMAT;
    }

    return ret;
}

RESULT AutoCompleteFormat::update(IMAGEINFO &predict)
{
    RESULT ret = RESULT::SUCCESS;

    int weight = 0;
    int target_index = -1;
    for (auto index : m_candidate)
    {
        if (weight < get<CONFSTRUCTINDEX::WEIGHT>(m_originlist[index]))
        {
            weight = get<CONFSTRUCTINDEX::WEIGHT>(m_originlist[index]);
            target_index = index;
        }
    }

    if (target_index >= 0)
    {
        predict.format.height = std::stoi(get<CONFSTRUCTINDEX::HEIGHT>(m_originlist[target_index]), nullptr);
        predict.format.width = std::stoi(get<CONFSTRUCTINDEX::WIDTH>(m_originlist[target_index]), nullptr);
        predict.format.stride = std::stoi(get<CONFSTRUCTINDEX::STRIDE>(m_originlist[target_index]), nullptr);
        ret = RESULT::SUCCESS;
    }
    else
    {
        ret = RESULT::INVALID_FORMAT;
    }

    return ret;
}

RESULT AutoCompleteFormat::autocomplete(IMAGEINFO &predict, IMAGEINFO &src, const SOURCETYPE type)
{
    RESULT ret = RESULT::SUCCESS;

    m_candidate.clear();
    string height = to_string(src.format.height);
    string width = to_string(src.format.width);
    string stride = to_string(src.format.stride);
    CONFSTRUCT src_tuple(height, width, stride, 1);

    ret = parse(src_tuple, type);
    ret = update(predict);

    return ret;
}

BOOL AutoCompleteFormat::check(int &h, int &w)
{
    for (auto line : m_originlist)
    {
        string height_ori, width_ori, stride_ori;
        int weight_ori;
        std::tie(height_ori, width_ori, stride_ori, weight_ori) = line;

        if ((h == std::stoi(height_ori, nullptr)) && (w == std::stoi(width_ori, nullptr)))
        {
            return true;
        }
    }
    return false;
}

//这里会修改config
RESULT AutoCompleteFormat::add(IMAGEINFO &src)
{
    RESULT ret = RESULT::SUCCESS;

    m_candidate.clear();
    string height = to_string(src.format.height);
    string width = to_string(src.format.width);
    string stride = to_string(src.format.stride);
    CONFSTRUCT src_tuple(height, width, stride, 1);

    //如果全部匹配，则对应权重（分数）的+1
    if (RESULT::SUCCESS == parse(src_tuple, SOURCETYPE::HEIGHT) &&
        RESULT::SUCCESS == parse(src_tuple, SOURCETYPE::WIDTH) &&
        RESULT::SUCCESS == parse(src_tuple, SOURCETYPE::STRIDE))
    {
        if (m_candidate.size() > 0)
            get<CONFSTRUCTINDEX::WEIGHT>(m_originlist[m_candidate[0]]) += 1;
    }
    else
    {
        m_originlist.emplace_back(src_tuple);
    }

    dump();
}

RESULT AutoCompleteFormat::initInfo(IMAGEINFO &predict)
{
    RESULT ret = RESULT::SUCCESS;
    pair<int, int> weightmax(-1, -1);
    int index = 0;
    for (auto line : m_originlist)
    {
        string height_ori, width_ori, stride_ori;
        int weight_ori;
        std::tie(height_ori, width_ori, stride_ori, weight_ori) = line;

        if (weight_ori > weightmax.first)
        {
            weightmax.first = weight_ori;
            weightmax.second = index;
        }

        index++;
    }

    if (weightmax.second >= 0)
    {
        predict.format.height = std::stoi(get<CONFSTRUCTINDEX::HEIGHT>(m_originlist[weightmax.second]), nullptr);
        predict.format.width = std::stoi(get<CONFSTRUCTINDEX::WIDTH>(m_originlist[weightmax.second]), nullptr);
        predict.format.stride = std::stoi(get<CONFSTRUCTINDEX::STRIDE>(m_originlist[weightmax.second]), nullptr);
    }

    return ret;
}