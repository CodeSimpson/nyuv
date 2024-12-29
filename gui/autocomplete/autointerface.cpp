#include "defines.h"
#include "autocomplete_format.h"
#include "autocomplete_utils.h"
#include "autocomplete_config.h"

RESULT autocomplete_format(IMAGEINFO &predict, IMAGEINFO &src, const SOURCETYPE type)
{
    RESULT ret = RESULT::SUCCESS;
    if (AutoCompleteFormat::getInstance())
        ret = AutoCompleteFormat::getInstance()->autocomplete(predict, src, type);
    return ret;
}

RESULT autocomplete_add(IMAGEINFO &src)
{
    RESULT ret = RESULT::SUCCESS;
    if (AutoCompleteFormat::getInstance())
        ret = AutoCompleteFormat::getInstance()->add(src);
    return ret;
}

RESULT autocomplete_init(IMAGEINFO &predict)
{
    RESULT ret = RESULT::SUCCESS;
    if (AutoCompleteFormat::getInstance())
        ret = AutoCompleteFormat::getInstance()->initInfo(predict);
    return ret;
}

RESULT autocomplete_fileparse(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types)
{
    return parse_filename(out, file, types);
}

RESULT autocomplete_typeparse(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types)
{
    return parse_type(out, file, types);
}

vector<string> autocomplete_load(const CONFIG_TYPE &type)
{
    return Config::getInstance()->load(type);
}

void autocomplete_dump(vector<string> &result, const CONFIG_TYPE &type)
{
    Config::getInstance()->dump(result, type);
}