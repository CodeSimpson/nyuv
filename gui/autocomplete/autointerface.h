#ifndef __AUTOINTERFACE_H__
#define __AUTOINTERFACE_H__

#include "defines.h"
#include "autocomplete_format.h"
#include "autocomplete_utils.h"
#include "autocomplete_config.h"

RESULT autocomplete_format(IMAGEINFO &predict, IMAGEINFO &src, const SOURCETYPE type);
RESULT autocomplete_add(IMAGEINFO &src);
RESULT autocomplete_init(IMAGEINFO &predict);

RESULT autocomplete_fileparse(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types);
RESULT autocomplete_typeparse(IMAGEINFO &out, FILEINFO &file, std::vector<std::string> &types);

vector<string> autocomplete_load(const CONFIG_TYPE &);
void autocomplete_dump(vector<string> &, const CONFIG_TYPE &);

#endif