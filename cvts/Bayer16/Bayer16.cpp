#include "Bayer16.h"
#include "threadpool.h"
#include "table.h"

#undef __LOGTAG__
#define __LOGTAG__ "Bayer16"
#include "log.h"

#include <cmath>
#include <omp.h>

using namespace std;

namespace cvt
{

    Bayer16::Bayer16(const STR &type)
    {
        // LOGI("Bayer16 start.");
        if ((type == "Bayer10GRBG") ||
            (type == "Bayer10GBRG") ||
            (type == "Bayer10RGGB") ||
            (type == "Bayer10BGGR"))
        {
            m_type = BAYER16TYPE::Bayer10;
        }
        else if ((type == "Bayer12GRBG") ||
                 (type == "Bayer12GBRG") ||
                 (type == "Bayer12RGGB") ||
                 (type == "Bayer12BGGR"))
        {
            m_type = BAYER16TYPE::Bayer12;
        }
        else if ((type == "Bayer14GRBG") ||
                 (type == "Bayer14GBRG") ||
                 (type == "Bayer14RGGB") ||
                 (type == "Bayer14BGGR"))
        {
            m_type = BAYER16TYPE::Bayer14;
        }
        else if ((type == "Bayer16GRBG") ||
                 (type == "Bayer16GBRG") ||
                 (type == "Bayer16RGGB") ||
                 (type == "Bayer16BGGR"))
        {
            m_type = BAYER16TYPE::Bayer16;
        }
        else
        {
            m_type = BAYER16TYPE::Bayer16;
        }
    }

    Bayer16::~Bayer16()
    {
        // LOGI("Bayer16 done.");
    }

    VOID Bayer16::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width * 2;

        //Bayer8
        oformat.size = oformat.height * oformat.width;
    }

    VOID Bayer16::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width * 2;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    STR Bayer16::parseFormat(REQUESTINFO &requestInfo)
    {
        BUFFER &src = requestInfo.ibuffer;
        size_t size = requestInfo.ibuffer.size();
        if (size <= 2)
        {
            return "";
        }

        STR ret = "";

        //检查有效bit数
        {
            UCHAR pixel = 0x00;
            for (size_t pos = 0; pos < size; pos += 2)
            {
                pixel |= src[pos + 1]; //高位
            }

            if (pixel & 0xc0) //11000000
            {
                ret = "Bayer16";
            }
            else if (pixel & 0x30) //00110000
            {
                ret = "Bayer14";
            }
            else if (pixel & 0x0c) //00001100
            {
                ret = "Bayer12";
            }
            else if (pixel & 0x03) //00000011
            {
                ret = "Bayer10";
            }
        }

        //检查bayer排列格式
        {
            double P14 = .0;
            double P23 = .0;

            //width, height 一定是偶数
            size_t width = requestInfo.iformat.width;
            size_t height = requestInfo.iformat.height;
            size_t size = width * height * 2;
            size_t stride = requestInfo.iformat.stride;
            size_t valid_width = width * 2;

            //[-1, 0]
            //[ 0, 1] 卷积
            static const size_t STEP_PARSE_Y = 4 * 4;
            static const size_t STEP_PARSE_X = 4 * 4;
            for (size_t y = 0; y < height; y += STEP_PARSE_Y)
            {
                for (size_t x = 0; x < valid_width; x += STEP_PARSE_X)
                {
                    size_t pos;

                    pos = y * stride + x;
                    UINT32 P1 = src[pos] | (src[pos + 1] << 8);
                    pos = y * stride + x + 2;
                    UINT32 P2 = src[pos] | (src[pos + 1] << 8);
                    pos = (y + 1) * stride + x;
                    UINT32 P3 = src[pos] | (src[pos + 1] << 8);
                    pos = (y + 1) * stride + x + 2;
                    UINT32 P4 = src[pos] | (src[pos + 1] << 8);

                    P14 += (max(P1, P4) - min(P1, P4));
                    P23 += (max(P2, P3) - min(P2, P3));
                }
            }

            if (P14 < P23)
            {
                ret += "GRBG";
            }
            else
            {
                ret += "RGGB";
            }
        }

        return ret;
    }

    RESULT Bayer16::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width * 2;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }
        m_format = requestInfo.iformat;

        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    VOID Bayer16::process(BUFFER &dest, BUFFER &src)
    {
        //width, height 一定是偶数
        size_t width = m_format.width;
        size_t height = m_format.height;
        size_t size = width * height * 2;
        size_t stride = m_format.stride;

        BITS bits = BITS::BITS_16;
        switch (m_type)
        {
        case BAYER16TYPE::Bayer10:
            bits = BITS::BITS_10;
            break;
        case BAYER16TYPE::Bayer12:
            bits = BITS::BITS_12;
            break;
        case BAYER16TYPE::Bayer14:
            bits = BITS::BITS_14;
            break;
        case BAYER16TYPE::Bayer16:
        default:
            bits = BITS::BITS_16;
            break;
        }

        size_t valid_width = width * 2;
        auto process = [=, &dest, &src](size_t &index, const size_t &begin) {
            for (size_t y = 0; y < height; y++)
            {
                for (size_t x = begin; x < valid_width; x += 8)
                {
                    size_t pos = y * stride + x;
                    UINT32 in = src[pos] | (src[pos + 1] << 8);
                    dest[index] = To8Table(in, bits);
                    index += 4;
                }
            }
        };

        //采用log法转换为8bit；
        auto processP1 = [=, &dest, &src]() {
            LOGI("BAYER16 work T1");
            size_t index = 0;
            process(index, 0);
        };
        auto processP2 = [=, &dest, &src]() {
            LOGI("BAYER16 work T2");
            size_t index = 1;
            process(index, 2);
        };
        auto processP3 = [=, &dest, &src]() {
            LOGI("BAYER16 work T3");
            size_t index = 2;
            process(index, 4);
        };
        auto processP4 = [=, &dest, &src]() {
            LOGI("BAYER16 work T4");
            size_t index = 3;
            process(index, 6);
        };

        auto resultP1 = Thread::getThreadInstance()->enqueue(processP1);
        auto resultP2 = Thread::getThreadInstance()->enqueue(processP2);
        auto resultP3 = Thread::getThreadInstance()->enqueue(processP3);
        auto resultP4 = Thread::getThreadInstance()->enqueue(processP4);

        //wait
        resultP1.get();
        resultP2.get();
        resultP3.get();
        resultP4.get();

        m_format.stride = valid_width;
    }

} // namespace cvt