#ifndef __Bayer16_H__
#define __Bayer16_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{
    typedef enum class __BAYER16TYPE__
    {
        Bayer10,
        Bayer12,
        Bayer14,
        Bayer16
    } BAYER16TYPE;

    class Bayer16 : public CVT
    {
    public:
        Bayer16(const STR &type);
        virtual ~Bayer16();
        RESULT cvt(REQUESTINFO &requestInfo);
        STR parseFormat(REQUESTINFO &);
        BOOL supportParseFormat() { return true; }
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        BOOL testIBuffer(const FORMAT &format) {
            return !((format.height & 1 || format.width & 1) || (format.height < 4 || format.width < 4));
        }

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height * 2;
            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

    private:
        VOID process(BUFFER &dest, BUFFER &src);

    private:
        FORMAT m_format;
        BAYER16TYPE m_type;
    };

} //namespace cvt

#endif