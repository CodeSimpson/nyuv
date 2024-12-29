#ifndef __Bayer8_H__
#define __Bayer8_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{

    typedef enum class __BAYER8TYPE__
    {
        BGGR,
        GBRG,
        GRBG,
        RGGB,
    } BAYER8TYPE;

    class Bayer8 : public CVT
    {
    public:
        Bayer8(const STR &type);
        virtual ~Bayer8();
        RESULT cvt(REQUESTINFO &requestInfo);
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height;
            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

        BOOL testIBuffer(const FORMAT &format) {
            return !(format.height & 1 || format.width & 1);
        }

    private:
        VOID processV1(BUFFER &dest, BUFFER &src);
        VOID processV2(BUFFER &dest, BUFFER &src);
        VOID processV2G(BUFFER &dest, BUFFER &src);
        VOID processV2RB(BUFFER &dest, BUFFER &src);

    private:
        BAYER8TYPE m_cvtypeout;

        FORMAT m_format;
        BOOL m_bp;
    };

} //namespace cvt

#endif