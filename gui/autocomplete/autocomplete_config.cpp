#include "defines.h"
#include "autocomplete_config.h"
#include "autocomplete_utils.h"
#include "tinyxml2.h"

#include <string>
#include <iostream>

using namespace std;
using namespace tinyxml2;

//程序载入即初始化，解决autocomplete_format等依赖问题
Config::Garbo Config::garbo;
Config *Config::p_config_instance = new Config();
Config *Config::getInstance()
{
    return p_config_instance;
}

Config::Config()
{
    char const *home = CONFIG;
    m_config_path = string(home) + ".nyuv.config";
    load();
}

Config::~Config()
{
    dump();
}

void Config::load()
{
    m_doc.LoadFile(m_config_path.c_str());
    //开始解析xml
    XMLElement *nodelist = m_doc.RootElement();
    m_doc_path = nodelist->FirstChildElement("PATH");
    m_doc_format = nodelist->FirstChildElement("FORMAT");
    m_doc_filter = nodelist->FirstChildElement("FILTER");
    m_doc_version = nodelist->FirstChildElement("VERSION");

    m_doc_path_dump = m_doc_path;
    m_doc_format_dump = m_doc_format;
    m_doc_filter_dump = m_doc_filter;
    m_doc_version_dump = m_doc_version;
}

vector<string> Config::load(const CONFIG_TYPE &config_type)
{
    vector<string> result;
    switch (config_type)
    {
    case CONFIG_TYPE::PATH:
        loadPath(result);
        break;
    case CONFIG_TYPE::FORMAT:
        loadFormat(result);
        break;
    case CONFIG_TYPE::FILTER:
        loadFilter(result);
        break;
    case CONFIG_TYPE::VERSION:
        loadVersion(result);
        break;

    default:
        break;
    }

    return result;
}

void Config::loadVersion(vector<string> &result)
{
    if (m_doc_version)
    {
        string current_version = m_doc_version->FirstChildElement("CURRENT")->GetText();
        string ignore_version = m_doc_version->FirstChildElement("IGNORE")->GetText();
        result.emplace_back(current_version);
        result.emplace_back(ignore_version);
    }
    dumpVersion(result);
}

void Config::loadFilter(vector<string> &result)
{
    if (m_doc_filter)
    {
        string filter = m_doc_filter->GetText();
        split(filter, ' ', &result);
    }
    dumpFilter(result);
}

void Config::loadPath(vector<string> &result)
{
    vector<string> origin;
    XMLElement *node = m_doc_path->FirstChildElement("ROOTDIR");
    while (node)
    {
        string dir = node->GetText();
        origin.emplace_back(dir);
        if (dir == "$HOME")
        {
            dir = getenv("HOME");
        }
        result.emplace_back(dir);
        node = node->NextSiblingElement();
    }
    dumpPath(origin);
}

void Config::loadFormat(vector<string> &result)
{
    XMLElement *node = m_doc_format->FirstChildElement("CLASS");
    while (node)
    {
        string size = node->FirstChildElement("SIZE")->GetText();
        result.emplace_back(size);
        node = node->NextSiblingElement();
    }
    dumpFormat(result);
}

void Config::dump()
{
    if ((m_doc_path_dump != m_doc_path) ||
        (m_doc_format_dump != m_doc_format))
    {
        XMLElement *root = m_doc_dump.NewElement("BODY");
        m_doc_dump.InsertEndChild(root);

        if (m_doc_path_dump)
            root->InsertFirstChild(m_doc_path_dump);
        if (m_doc_format_dump)
            root->InsertFirstChild(m_doc_format_dump);
        if (m_doc_filter_dump)
            root->InsertFirstChild(m_doc_filter_dump);
        if (m_doc_version_dump)
            root->InsertFirstChild(m_doc_version_dump);
        m_doc_dump.SaveFile(m_config_path.c_str());
    }
}

void Config::dump(vector<string> &result, const CONFIG_TYPE &config_type)
{
    switch (config_type)
    {
    case CONFIG_TYPE::PATH:
        dumpPath(result);
        break;
    case CONFIG_TYPE::FORMAT:
        dumpFormat(result);
        break;
    case CONFIG_TYPE::FILTER:
        dumpFilter(result);
        break;
    case CONFIG_TYPE::VERSION:
        dumpVersion(result);
        break;

    default:
        break;
    }
}

void Config::dumpVersion(vector<string> &result)
{
    m_doc_version_dump = m_doc_dump.NewElement("VERSION");
    XMLElement *ele_current = m_doc_dump.NewElement("CURRENT");
    XMLText *ele_current_text = m_doc_dump.NewText(result[0].c_str());
    ele_current->InsertEndChild(ele_current_text);
    XMLElement *ele_ignore = m_doc_dump.NewElement("IGNORE");
    XMLText *ele_ignore_text = m_doc_dump.NewText(result[1].c_str());
    ele_ignore->InsertEndChild(ele_ignore_text);
    m_doc_version_dump->InsertEndChild(ele_current);
    m_doc_version_dump->InsertEndChild(ele_ignore);
}

void Config::dumpPath(vector<string> &result)
{
    m_doc_path_dump = m_doc_dump.NewElement("PATH");
    for (auto line : result)
    {
        XMLElement *ele_rootdir = m_doc_dump.NewElement("ROOTDIR");

        XMLText *ele_rootdir_text = m_doc_dump.NewText(line.c_str());
        ele_rootdir->InsertEndChild(ele_rootdir_text);
        m_doc_path_dump->InsertEndChild(ele_rootdir);
    }
}

void Config::dumpFilter(vector<string> &result)
{
    m_doc_filter_dump = m_doc_dump.NewElement("FILTER");
    string filter = "";
    size_t index = 0;
    for (auto f : result)
    {
        if (index == 0)
        {
            filter += f;
        }
        else
        {
            filter += (" " + f);
        }
        index++;
    }
    XMLText *ele_rootdir_text = m_doc_dump.NewText(filter.c_str());
    m_doc_filter_dump->InsertEndChild(ele_rootdir_text);
}

void Config::dumpFormat(vector<string> &result)
{
    m_doc_format_dump = m_doc_dump.NewElement("FORMAT");
    for (auto line : result)
    {
        XMLElement *ele_class = m_doc_dump.NewElement("CLASS");
        XMLElement *ele_size = m_doc_dump.NewElement("SIZE");
        XMLElement *ele_type = m_doc_dump.NewElement("TYPE");

        XMLText *ele_size_text = m_doc_dump.NewText(line.c_str());
        ele_size->InsertEndChild(ele_size_text);

        ele_class->InsertEndChild(ele_size);
        ele_class->InsertEndChild(ele_type);
        m_doc_format_dump->InsertEndChild(ele_class);
    }
}
