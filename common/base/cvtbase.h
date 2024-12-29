/*
 * @Author: your name
 * @Date: 2020-06-30 10:03:09
 * @LastEditTime: 2020-08-24 20:13:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /nyuv/common/base/cvtbase.h
 */
#ifndef __CVTBASE_H__
#define __CVTBASE_H__

#include <string>
#include <iostream>
#include <cmath>

#include "defines.h"

/***
 * cvt中的每个方法，必须继承自CVT
 * **/
class CVT
{
public:
    CVT() {}
    virtual ~CVT() {}

    //REQUESTINFO的内容或许会增加
    virtual RESULT cvt(REQUESTINFO &) = 0;

    //每个cvt node明确知道自己的职能，或许输入图到数出图大小不会变，或许会变，
    //这都在setOBufferInfo中体现
    //但是cvt node只能改变输出，不能改变输入
    virtual VOID setOBufferInfo(FORMAT &, const FORMAT &) = 0;

    //获取cvt node输入的buffer信息, 已知w h s, 计算size
    virtual VOID getIBufferInfo(FORMAT &) = 0;

    //测试buffer格式是否符合要求
    virtual BOOL testIBuffer(const FORMAT &format) {
        return !(format.height == 0 || format.width == 0);
    }

    //通过format猜测格式
    virtual FLOAT parseScoreBySize(const FORMAT &format) = 0;

    //获取有效的bit数和格式
    //对于16bit图像有效, 比如Bayer16-12/Bayer16-14
    virtual STR parseFormat(REQUESTINFO &req) { return ""; }

    //是否支持格式推导
    virtual BOOL supportParseFormat() { return false; }

protected:
    /***
     * target = (tar, tar)
     * current = (tar, current)
     * score = T * C / (|T| * |C|)
     * 获取余xian值, 正的, 接近1最好
     * **/
    FLOAT coScore(const FLOAT &current, const FLOAT &target){
        FLOAT target_vec[2] = {target, target};
        FLOAT current_vec[2] = {target, current};
        FLOAT p1 = target_vec[0] * current_vec[0];
        FLOAT p2 = target_vec[1] * current_vec[1];
        FLOAT p3 = target_vec[0] * 1.414;
        FLOAT p4 = sqrt(p1 + current_vec[1] * current_vec[1]);
        FLOAT score = (p1 + p2) / (p3 * p4);
        return score;
    }
};

typedef CVT *(*create_t)(const STR &);
typedef VOID (*destroy_t)(CVT *);

typedef struct __CALLBACKS__
{
    create_t create;
    destroy_t destroy;
} CALLBACKS;

typedef VOID (*CVTENTRY)(CALLBACKS &);

#endif