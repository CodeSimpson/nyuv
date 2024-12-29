#ifndef __AUTOCOMPLETE_CONFIG_H__
#define __AUTOCOMPLETE_CONFIG_H__

#include "tinyxml2.h"

#include <string>
#include <vector>

using namespace std;
using namespace tinyxml2;

enum class CONFIG_TYPE
{
    INVALID,
    PATH,
    FORMAT,
    FILTER,
    VERSION,
};

//Config class 管理配置文件，全局仅一份
//liblist.xml不算配置文件，因为它是确定的
class Config
{
public:
    static Config* getInstance();
    virtual ~Config();
    vector<string> load(const CONFIG_TYPE &);
    void dump(vector<string> &, const CONFIG_TYPE &);

private:
    class Garbo
    {
    public:
        ~Garbo()
        {
            if (p_config_instance)
            {
                delete p_config_instance;
            }
        }
    };
    static Garbo garbo;
    static Config *p_config_instance;
    Config();

    void load();
    void loadPath(vector<string> &);
    void loadFormat(vector<string> &);
    void loadFilter(vector<string> &);
    void loadVersion(vector<string> &);

    void dump();
    void dumpPath(vector<string> &);
    void dumpFormat(vector<string> &);
    void dumpFilter(vector<string> &);
    void dumpVersion(vector<string> &);

private:
    string m_config_path;

    XMLDocument m_doc;
    XMLElement *m_doc_path;
    XMLElement *m_doc_format;
    XMLElement *m_doc_filter;
    XMLElement *m_doc_version;

    XMLDocument m_doc_dump;
    XMLElement *m_doc_path_dump;
    XMLElement *m_doc_format_dump;
    XMLElement *m_doc_filter_dump;
    XMLElement *m_doc_version_dump;
};

#endif