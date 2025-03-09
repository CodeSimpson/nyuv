#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <string>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <tuple>
#include <sstream>

#include "buffer.h"
#include "tools.h"

#if defined(__GNUC__)
#define CDK_VISIBILITY_PUBLIC __attribute__((visibility("default")))
#define CDK_VISIBILITY_LOCAL __attribute__((visibility("hidden")))
#else
#define CDK_VISIBILITY_PUBLIC __declspec(dllexport)
#define CDK_VISIBILITY_LOCAL
#endif // defined(__GNUC__)

#ifdef INSTALL
#define LIBLIST "/usr/lib/nyuv/liblist.xml"
#define LIBROOT "/usr/lib/nyuv/"
#define CONFIG "/etc/nyuv/"
#else
#define LIBLIST "../lib/liblist.xml"
#define LIBROOT "../lib/"
#define CONFIG "./"
#endif

typedef int INT32;
typedef unsigned int UINT32;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef bool BOOL;
typedef void VOID;
typedef float FLOAT;
typedef unsigned short USHORT;
typedef std::string STR;
typedef std::atomic<BOOL> ATOMIC_BOOL;

#define PI 3.14159265

#define SAFEDEL(p)      \
    {                   \
        if (p)          \
            delete (p); \
        (p) = NULL;     \
    }

/***
 * return类型
 * 尽量把所有的return类型加在这里，并且之使用这里的return状态
 * 以便后续管理
 * **/
typedef enum class __RESULT__
{
    FAILED = 0,
    SUCCESS = 65535,
    INVALID_FILE = 1,
    INVALID_TYPE = 2,
    INVALID_LIB = 3,
    INVALID_FORMAT = 4,
    INVALID_BUFFER = 5,
} RESULT;

static std::string parseResult(const RESULT &result)
{
    switch (result)
    {
    case RESULT::FAILED:
        return "未知错误!";
    case RESULT::INVALID_FILE:
        return "无法打开文件!";
    case RESULT::INVALID_TYPE:
        return "图像类型选择不正确!";
    case RESULT::INVALID_LIB:
        return "内部错误, 非法库!";
    case RESULT::INVALID_FORMAT:
        return "SIZE不符合要求!请检查FORMAT!";
    case RESULT::INVALID_BUFFER:
        return "内部错误, 非法内存!";
    }
}

/***
 * 图象格式，应包含：
 * 格式、对其方式、每个像素的bit数
 * **/
typedef enum class __TYPE__
{
    BLOB = 0, //对应最后生成的图片，可以是png、jpeg等直接可以打开的格式
    RGB888 = 1,
    RGBA8888 = 2,
    YUV420SPNV12 = 3,
    YUV420SPNV21 = 4,
    BAYER8888BGGR = 5,
    BAYER8888GBRG = 6,
    BAYER8888GRBG = 7,
    BAYER8888RGGB = 8,
    MIPI10BGGR = 9,
    MIPI10GBRG = 10,
    MIPI10GRBG = 11,
    MIPI10RGGB = 12,
    RGB16 = 13,
    RGBA16 = 14,
    ARGB16 = 15,
    RGB14 = 16,
    RGBA14 = 17,
    ARGB14 = 18,
    ANY = -2,
    INVALID_TYPE = -1
} TYPE;

/***
 * 图像格式
 * **/
typedef struct __FORMAT__
{
    TYPE type;              // raw格式或者yuv格式
    UINT32 height;
    UINT32 width;
    UINT32 stride;
    size_t size;

    bool isValid()
    {
        if (height <= 0)
            return false;
        if (width <= 0)
            return false;
        if (stride <= 0)
            return false;
        return true;
    }

    void operator=(const __FORMAT__ &instance)
    {
        type = instance.type;
        height = instance.height;
        width = instance.width;
        stride = instance.stride;
        size = instance.size;
    }

    __FORMAT__()
    {
        type = __TYPE__::INVALID_TYPE;
        width = 0;
        height = 0;
        stride = 0;
        size = 0;
    }

    __FORMAT__(const __FORMAT__ &another)
    {
        type = another.type;
        width = another.width;
        height = another.height;
        stride = another.stride;
        size = another.size;
    }

    friend std::ostream &operator<<(std::ostream &out, const __FORMAT__ &format)
    {
        out << "w h s: (" << format.width << ", " << format.height << ", " << format.stride << ")";

        return out;
    }

    BOOL operator==(const __FORMAT__ &instance)
    {
        return std::tie(type, width, height, stride, size) == std::tie(instance.type, instance.width, instance.height, instance.stride, instance.size);
    }

} FORMAT;

/***
 * link表示cvt中一个方法的link关系
 * src表示输入格式
 * dest表示输出格式
 * 有了link关系后，添加新的cvt方法时，输出格式只需要转换成已有的cvt方法可支持的输入格式就行了
 * **/
//TODO： 目前不是用link实现的，之后需要转换成link实现
typedef struct __LINK__
{
    TYPE src;
    TYPE dest;

    void operator=(const __LINK__ &instance)
    {
        src = instance.src;
        dest = instance.dest;
    }
} LINK;

/***
 * 输入和输出，这是对接口而言的
 * 上层应至少填写filename、format这两个成员，其他成员在底层填写（TODO：link、buffer）
 * **/
typedef struct __REQUESTINFO__
{
    const char *ifilename;
    const char *ofilename;
    FORMAT iformat;
    FORMAT oformat;
    BUFFER ibuffer;
    BUFFER obuffer;
    LINK link;
    BOOL bayer;

    __REQUESTINFO__()
    {
        ifilename = "";
        ofilename = "";
        bayer = false;
    }

    __REQUESTINFO__(__REQUESTINFO__ &instance)
    {
        ifilename = instance.ifilename;
        ofilename = instance.ofilename;
        iformat = instance.iformat;
        oformat = instance.oformat;
        ibuffer = instance.ibuffer;
        obuffer = instance.obuffer;
        link = instance.link;
        bayer = instance.bayer;
    }

    VOID operator=(__REQUESTINFO__ &instance)
    {
        ifilename = instance.ifilename;
        ofilename = instance.ofilename;
        iformat = instance.iformat;
        oformat = instance.oformat;
        ibuffer = instance.ibuffer;
        obuffer = instance.obuffer;
        link = instance.link;
        bayer = instance.bayer;
    }

    VOID swap()
    {
        std::swap(iformat, oformat);
        ibuffer.swap(obuffer);
    }

} REQUESTINFO;

typedef struct __IMAGEINFO__
{
    std::string type;       // 图像格式string
    FORMAT format;
    BOOL bayer;

    __IMAGEINFO__()
    {
        bayer = false;
    }
    __IMAGEINFO__(const std::string &type_, const FORMAT &format_)
        : type(type_), format(format_)
    {
        ;
    }

    VOID operator=(const __IMAGEINFO__ &instance)
    {
        type = instance.type;
        format = instance.format;
        bayer = instance.bayer;
    }

    BOOL operator==(const __IMAGEINFO__ &instance)
    {
        return std::tie(type, format) == std::tie(instance.type, instance.format);
    }

} IMAGEINFO;

template <typename T = UCHAR>
class __P3__
{
public:
    //只希望定义一个类，并且成员有不同的名字
    union
    {
        struct
        {
            T P1, P2, P3;
        };
        struct
        {
            T Y, U, V;
        };
        struct
        {
            T R, G, B;
        };
        struct
        {
            T H, S, _V;
        };
    };

public:
    __P3__(const T &_P1, const T &_P2, const T &_P3) : P1(_P1), P2(_P2), P3(_P3) {}
    __P3__() : P1(0), P2(0), P3(0) {}
    __P3__(const __P3__<> &p3f) : P1(p3f.P1), P2(p3f.P2), P3(p3f.P3) {}
    virtual ~__P3__() {}

    VOID operator=(const __P3__ &p3f)
    {
        P1 = p3f.P1;
        P2 = p3f.P2;
        P3 = p3f.P3;
    }
    VOID operator+=(const __P3__ &p3f)
    {
        P1 += p3f.P1;
        P2 += p3f.P2;
        P3 += p3f.P3;
    }
    VOID operator*=(const T &rate)
    {
        P1 *= rate;
        P2 *= rate;
        P3 *= rate;
    }
    VOID operator/=(const T &rate)
    {
        P1 /= rate;
        P2 /= rate;
        P3 /= rate;
    }

    static __P3__ convert(const __P3__<> &p3f)
    {
        return __P3__<T>(p3f);
    }

    T max() const
    {
        return tools::max(P1, P2, P3);
    }

    T min() const
    {
        return tools::min(P1, P2, P3);
    }

    friend std::ostream &operator<<(std::ostream &out, const __P3__ &p3f)
    {
        out << "(" << p3f.P1 << ", " << p3f.P2 << ", " << p3f.P3 << ")";
        return out;
    }
};
template <typename T = UCHAR>
using YUV = __P3__<T>;
template <typename T = UCHAR>
using RGB = __P3__<T>;
template <typename T = UCHAR>
using HSV = __P3__<T>;

#endif