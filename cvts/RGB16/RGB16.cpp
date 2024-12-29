#include <cstring>

#include "RGB16.h"
#include "threadpool.h"
#include "table.h"

#undef __LOGTAG__
#define __LOGTAG__ "RGB16"
#include "log.h"

using namespace std;

namespace cvt
{

    RGB16::RGB16(const STR &type) : m_enbale_ARGB(false), m_enbale_RGBA(false)
    {
        // LOGI("RGB16 start.");

        if (type == "RGB16")
        {
            m_cvtypeout = RGB16TYPE::RGB16;
        }
        else if (type == "RGBA16")
        {
            m_cvtypeout = RGB16TYPE::RGBA16;
            enableRGBA();
        }
        else if (type == "ARGB16")
        {
            m_cvtypeout = RGB16TYPE::ARGB16;
            enableARGB();
        }
        else if (type == "UPRGB14")
        {
            m_cvtypeout = RGB16TYPE::UPRGB14;
        }
        else if (type == "UPRGBA14")
        {
            m_cvtypeout = RGB16TYPE::UPRGBA14;
            enableRGBA();
        }
        else if (type == "UPARGB14")
        {
            m_cvtypeout = RGB16TYPE::UPARGB14;
            enableARGB();
        }
    }

    RGB16::~RGB16()
    {
        // LOGI("RGB16 done.");
    }

    VOID RGB16::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width * 3;

        //RGB888, w * h * 3
        oformat.size = oformat.height * oformat.width * 3;
    }

    VOID RGB16::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width * 3 * 2;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    RESULT RGB16::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width * (isARGBEnable() || isRGBAEnable()) ? 8 : 6;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;

        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    inline VOID RGB16::enableARGB()
    {
        m_enbale_ARGB = true;
    }
    inline VOID RGB16::enableRGBA()
    {
        m_enbale_RGBA = true;
    }

    inline size_t RGB16::isARGBEnable()
    {
        return m_enbale_ARGB ? 1 : 0;
    }
    inline size_t RGB16::isRGBAEnable()
    {
        return m_enbale_RGBA ? 1 : 0;
    }

    VOID RGB16::process(BUFFER &dest, BUFFER &src)
    {
        size_t width = m_format.width;
        size_t height = m_format.height;
        size_t stride = m_format.stride;
        size_t size = width * height;

        BITS bits = BITS::BITS_16;
        switch (m_cvtypeout)
        {
        case RGB16TYPE::RGB16:
        case RGB16TYPE::RGBA16:
        case RGB16TYPE::ARGB16:
        default:
            bits = BITS::BITS_16;
            break;

        case RGB16TYPE::UPRGB14:
        case RGB16TYPE::UPRGBA14:
        case RGB16TYPE::UPARGB14:
            bits = BITS::BITS_14;
            break;
        }

        const size_t step = (3 + isARGBEnable() + isRGBAEnable()) * 2;
        const size_t valid_width = width * step;
        auto process = [=, &dest, &src](size_t &index, const size_t &begin) {
            for (size_t y = 0; y < height; y++)
            {
                for (size_t x = begin; x < valid_width; x += step)
                {
                    size_t pos = y * stride + x;
                    UINT32 in = src[pos] | (src[pos + 1] << 8);
                    dest[index] = To8Table(in, bits);
                    index += 3;
                }
            }
        };

        //主要区分ARGB 和 RGBA，unpacked都一样
        auto processR = [=, &dest, &src] {
            LOGI("RGB16 work T1");
            size_t index = 0;
            process(index, (isARGBEnable() + 0) * 2);
        };
        auto processG = [=, &dest, &src] {
            LOGI("RGB16 work T2");
            size_t index = 1;
            process(index, (isARGBEnable() + 1) * 2);
        };
        auto processB = [=, &dest, &src] {
            LOGI("RGB16 work T3");
            size_t index = 2;
            process(index, (isARGBEnable() + 2) * 2);
        };

        //60ms, for 5792 x 4344
        //30ms,只取高字节
        auto resultR = Thread::getThreadInstance()->enqueue(processR);
        auto resultG = Thread::getThreadInstance()->enqueue(processG);
        auto resultB = Thread::getThreadInstance()->enqueue(processB);

        //wait
        resultR.get();
        resultG.get();
        resultB.get();

        m_format.stride = valid_width;
    } // namespace cvt

} //namespace cvt
