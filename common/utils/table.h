#ifndef __TABLE_H__
#define __TABLE_H__

#include "defines.h"

typedef enum class __BITS__
{
    BITS_10 = 0,
    BITS_12 = 1,
    BITS_14 = 2,
    BITS_16 = 3,
    BITS_MAX
} BITS;

const UCHAR &To8Table(const UINT32 &data, const BITS &mask);
const RGB<> &YUV2RGBTable(const YUV<> &data);
const RGB<> &HSV2RGBTable(const HSV<> &data);
const HSV<> &RGB2HSVTable(const RGB<> &data);

#endif