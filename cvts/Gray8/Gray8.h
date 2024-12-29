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
    class Gray8 : public CVT
    {
    public:
        Gray8(const STR &type);
        virtual ~Gray8();
        RESULT cvt(REQUESTINFO &requestInfo);
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height;
            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

    private:
        VOID process(BUFFER &dest, BUFFER &src);

    private:
        FORMAT m_format;
    };

} //namespace cvt

#endif