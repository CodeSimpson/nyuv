#include "MIPI.h"
#include "threadpool.h"
#include "table.h"

#undef __LOGTAG__
#define __LOGTAG__ "MIPI"
#include "log.h"

#include <cmath>

using namespace std;

namespace cvt
{

    MIPI::MIPI(const STR &type)
    {
        if ((type == "MIPI10GRBG") ||
            (type == "MIPI10GBRG") ||
            (type == "MIPI10RGGB") ||
            (type == "MIPI10BGGR"))
        {
            m_type = MIPITYPE::MIPI10;
        }
        else if ((type == "MIPI12GRBG") ||
                 (type == "MIPI12GBRG") ||
                 (type == "MIPI12RGGB") ||
                 (type == "MIPI12BGGR"))
        {
            m_type = MIPITYPE::MIPI12;
        }
        else if ((type == "MIPI14GRBG") ||
                 (type == "MIPI14GBRG") ||
                 (type == "MIPI14RGGB") ||
                 (type == "MIPI14BGGR"))
        {
            m_type = MIPITYPE::MIPI14;
        }
        else
        {
            m_type = MIPITYPE::MIPI10;
        }

        // LOGI("MIPI start.");
    }

    MIPI::~MIPI()
    {
        // LOGI("MIPI done.");
    }

    VOID MIPI::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width;

        //Bayer16, w * h * 2
        oformat.size = oformat.height * oformat.stride;
    }

    VOID MIPI::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width * 5 / 4;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    STR MIPI::parseFormat(REQUESTINFO &requestInfo)
    {
        BUFFER &src = requestInfo.ibuffer;
        size_t size = requestInfo.ibuffer.size();
        if (size <= 2)
        {
            return "";
        }

        STR ret = "";

        //检查有效bit数
        size_t gap = 5;
        {
            size_t width = requestInfo.iformat.width;
            size_t height = requestInfo.iformat.height;
            size_t stride = requestInfo.iformat.stride;
            // mipi10
            size_t base_stride = 4 * stride;
            if (base_stride >= 5 * width)
            {
                ret = "MIPI10";
                gap = 5;
            }
            // mipi12
            if (base_stride >= 6 * width)
            {
                ret = "MIPI12";
                gap = 6;
            }
            // mipi14
            if (base_stride >= 7 * width)
            {
                ret = "MIPI14";
                gap = 7;
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
            size_t valid_width = stride;

            //[-1, 0]
            //[ 0, 1] 卷积
            for (size_t y = 0; y < height; y += gap)
            {
                for (size_t x = 0; x < valid_width; x += gap)
                {
                    size_t pos;

                    pos = y * stride + x;
                    UINT32 P1 = src[pos];
                    pos = y * stride + x + 1;
                    UINT32 P2 = src[pos];
                    pos = (y + 1) * stride + x;
                    UINT32 P3 = src[pos];
                    pos = (y + 1) * stride + x + 1;
                    UINT32 P4 = src[pos];

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

    RESULT MIPI::cvt(REQUESTINFO &requestInfo)
    {
        m_required_size = requestInfo.iformat.height * requestInfo.iformat.width * 5 / 4;
        if (m_required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", m_required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;

        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    VOID MIPI::process(BUFFER &dest, BUFFER &src)
    {
        int index = 0;
        size_t width = m_format.width;
        size_t height = m_format.height;
        size_t stride = m_format.stride;

        size_t size = m_required_size;

        UCHAR bit = 1;
        UCHAR mask = 10;
        BITS bits = BITS::BITS_10;
        switch (m_type)
        {
        default:
        case MIPITYPE::MIPI10:
            bit = 1;
            mask = 10;
            bits = BITS::BITS_10;
            break;
        case MIPITYPE::MIPI12:
            bit = 2;
            mask = 12;
            bits = BITS::BITS_12;
            break;
        case MIPITYPE::MIPI14:
            bit = 3;
            mask = 14;
            bits = BITS::BITS_14;
            break;
        }

        const UCHAR big_end[4] = {UCHAR(6 * bit), UCHAR(4 * bit), UCHAR(2 * bit), 0};
        const UCHAR and_mask = 0xFF >> (2 * (4 - bit));
        const UCHAR shift = 2 * bit;
        const UCHAR skip = 4 + bit;
        size_t valid_width = width * skip / 4;

        auto process = [=, &dest, &src](size_t &index, const size_t &begin) {
            const size_t pack_pos = (MIPITYPE::MIPI12 != m_type) ? (4 - begin) : (2 - begin % 2);
            const size_t mask_pos = (MIPITYPE::MIPI12 != m_type) ? begin : (3 - begin % 2);
            const size_t begin_begin = (MIPITYPE::MIPI12 != m_type) ? 0 : begin / 2;
            for (size_t y = 0; y < height; y++)
            {
                size_t ys = y * stride;
                for (size_t x = begin + begin_begin; x < valid_width; x += skip)
                {
                    size_t pos = ys + x;
                    UINT32 p1 = (((UINT32)src[pos]) << shift);
                    UINT32 p2 = (((src[pos + pack_pos] >> big_end[mask_pos]) & and_mask));
                    UINT32 in = p1 | p2;
                    dest[index] = To8Table(in, bits);
                    index += 4;
                }
            }
        };

        //采用log法转换为8bit；
        auto processP1 = [=, &dest, &src]() {
            LOGI("MIPI{} work T1", mask);
            //output的起始位置index
            size_t index = 0;
            process(index, 0);
        };
        auto processP2 = [=, &dest, &src]() {
            LOGI("MIPI work T2");
            size_t index = 1;
            process(index, 1);
        };
        auto processP3 = [=, &dest, &src]() {
            LOGI("MIPI work T3");
            size_t index = 2;
            process(index, 2);
        };
        auto processP4 = [=, &dest, &src]() {
            LOGI("MIPI work T4");
            size_t index = 3;
            process(index, 3);
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