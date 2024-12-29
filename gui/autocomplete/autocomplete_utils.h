#ifndef __AUTOCOMPLETE_UTILS_H__
#define __AUTOCOMPLETE_UTILS_H__

#include "defines.h"

#include <iostream>
#include <vector>

struct FILEINFO
{
    int64_t size;
    std::string suffix;
    std::string filename;
};

RESULT parse_filename(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types);
RESULT parse_type(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types);
VOID split(std::string &s, const char delim, std::vector<std::string> *ret);

#endif