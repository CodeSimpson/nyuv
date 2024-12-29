#ifndef __RGB16_H__
#define __RGB16_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{

    typedef enum class __RGB16TYPE__
    {
        RGB16,
        RGBA16,
        ARGB16,
        UPRGB14, //unpacked
        UPRGBA14,
        UPARGB14,
    } RGB16TYPE;

    class RGB16 : public CVT
    {
    public:
        RGB16(const STR &type);
        virtual ~RGB16();
        RESULT cvt(REQUESTINFO &requestInfo);
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height * 6;
            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

    private:
        VOID process(BUFFER &dest, BUFFER &src);
        size_t isARGBEnable();
        size_t isRGBAEnable();
        VOID enableARGB();
        VOID enableRGBA();

    private:
        ATOMIC_BOOL m_enbale_ARGB;
        ATOMIC_BOOL m_enbale_RGBA;

        FORMAT m_format;
        RGB16TYPE m_cvtypeout;
    };

} //namespace cvt

#endif