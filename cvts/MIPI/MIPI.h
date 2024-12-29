#ifndef __MIPI_H__
#define __MIPI_H__

#include <fstream>
#include <iostream>
#include <string>

#include "defines.h"
#include "cvtbase.h"

using namespace std;

namespace cvt
{

    typedef enum class __MIPITYPE__
    {
        MIPI10,
        MIPI12,
        MIPI14
    } MIPITYPE;

    class MIPI : public CVT
    {
    public:
        MIPI(const STR &type);
        virtual ~MIPI();
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
            size_t target_size = [&]() -> size_t {
                switch (m_type)
                {
                case MIPITYPE::MIPI10:
                    return format.width * format.height * 5 / 4;
                case MIPITYPE::MIPI12:
                    return format.width * format.height * 6 / 4;
                case MIPITYPE::MIPI14:
                    return format.width * format.height * 7 / 4;

                default:
                    return format.width * format.height * 5 / 4;
                }
            }();

            size_t current_size = format.size;
            return coScore(current_size, target_size);
        }

    private:
        VOID process(BUFFER &dest, BUFFER &src);

    private:
        FORMAT m_format;
        MIPITYPE m_type;
        size_t m_required_size;
    };

} //namespace cvt

#endif