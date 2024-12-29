#ifndef __YUV420SP_H__
#define __YUV420SP_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{

    typedef enum class __YUV420SPTYPE__
    {
        NV12,
        NV21,
    } YUV420SPTYPE;

    class YUV420SP : public CVT
    {
    public:
        YUV420SP(const STR& type);

        virtual ~YUV420SP();

        RESULT cvt(REQUESTINFO &requestInfo);
        VOID setOBufferInfo(FORMAT &oformat, const FORMAT &iformat);
        VOID getIBufferInfo(FORMAT &);

        FLOAT parseScoreBySize(const FORMAT &format)
        {
            size_t target_size = format.width * format.height * 3 / 2;
            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

    private:
        VOID process(BUFFER &dest, BUFFER &src);

    private:
        FORMAT m_format;
        YUV420SPTYPE m_cvtypeout;
    };

} //namespace cvt



#endif