#ifndef __CVTINTERFACE_H__
#define __CVTINTERFACE_H__

#include <string>
#include <map>

#include "defines.h"

void cvt_schedul_init();
void cvt_schedul_uninit();

/**
 * 上层调用这个函数，底层不关心数据来源、数据去向；
 * 底层会尽量做参数检查，但是上层也应该关系输入参数。
 * 
 * 上层从dst中获取结果
 * 
 * @type 输入格式
 * @src 输入图像信息
 * @dst 输出图像信息
 * 
 * type一定是liblist.xml中<source>标签的值
 * src的filename和format由用户输入
 * dst中的filename和format也是由用户输入，不过dst的format的height和width应该是和src的w和h相等的
 * **/
RESULT cvt_process(const std::string &type, REQUESTINFO &requestInfo);

/**
 * 格式检测
 * 通过分析Raw图数据, 得出文件的更加准确的格式, 比如valid bit和bayer partern
 * **/
STR cvt_parseFormat(const std::string &type, REQUESTINFO &requestInfo);

/**
 * 格式检测
 * 通过format size先分析出大概的格式
 * **/
typedef std::map<std::string, float> SCORE;
SCORE cvt_parseScoreBySize(const FORMAT& format);

/**
 * 当cvt_process的返回值不为SUCCESS的时候，调用cvt_error，将得到一些错误信息
 * **/
VOID cvt_error();

#endif