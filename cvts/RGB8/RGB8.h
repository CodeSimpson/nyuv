#ifndef __RGB8_H__
#define __RGB8_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{

    typedef enum class __RGB8TYPE__
    {
        RGB8,
        RGBA8,
        ARGB8,
    } RGB8TYPE;

    class RGB8 : public CVT
    {
    public:
        RGB8(const STR &type);
        virtual ~RGB8();
        RESULT cvt(REQUESTINFO &requestInfo);
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height * 3;
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
        RGB8TYPE m_cvtypeout;
    };

} //namespace cvt

#endif