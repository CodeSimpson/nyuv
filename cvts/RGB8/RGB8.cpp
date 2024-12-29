#include <cstring>

#include "RGB8.h"
#include "threadpool.h"
#include "table.h"

#undef __LOGTAG__
#define __LOGTAG__ "RGB8"
#include "log.h"

using namespace std;

namespace cvt
{

    RGB8::RGB8(const STR &type) : m_enbale_ARGB(false), m_enbale_RGBA(false)
    {
        // LOGI("RGB8 start.");

        if (type == "RGB8")
        {
            m_cvtypeout = RGB8TYPE::RGB8;
        }
        else if (type == "RGBA8")
        {
            m_cvtypeout = RGB8TYPE::RGBA8;
            enableRGBA();
        }
        else if (type == "ARGB8")
        {
            m_cvtypeout = RGB8TYPE::ARGB8;
            enableARGB();
        }
    }

    RGB8::~RGB8()
    {
        // LOGI("RGB8 done.");
    }

    VOID RGB8::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width * 3;

        //RGB888, w * h * 3
        oformat.size = oformat.height * oformat.width * 3;
    }

    VOID RGB8::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width * 3;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    RESULT RGB8::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width * (isARGBEnable() || isRGBAEnable()) ? 4 : 3;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;

        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    inline VOID RGB8::enableARGB()
    {
        m_enbale_ARGB = true;
    }
    inline VOID RGB8::enableRGBA()
    {
        m_enbale_RGBA = true;
    }

    inline size_t RGB8::isARGBEnable()
    {
        return m_enbale_ARGB ? 1 : 0;
    }
    inline size_t RGB8::isRGBAEnable()
    {
        return m_enbale_RGBA ? 1 : 0;
    }

    VOID RGB8::process(BUFFER &dest, BUFFER &src)
    {
        size_t width = m_format.width;
        size_t height = m_format.height;
        size_t stride = m_format.stride;
        size_t size = width * height;

        const size_t step = 3 + isARGBEnable() + isRGBAEnable();
        const size_t valid_width = width * step;
        auto process = [=, &dest, &src](size_t &index, const size_t &begin) {
            for (size_t y = 0; y < height; y++)
            {
                for (size_t x = begin; x < valid_width; x += step)
                {
                    size_t pos = y * stride + x;
                    dest[index] = src[pos];
                    index += 3;
                }
            }
        };
        //主要区分ARGB 和 RGBA
        auto processR = [=, &dest, &src] {
            LOGI("RGB8 work T1");
            size_t index = 0;
            process(index, (isARGBEnable() + 0));
        };
        auto processG = [=, &dest, &src] {
            LOGI("RGB8 work T2");
            size_t index = 1;
            process(index, (isARGBEnable() + 1));
        };
        auto processB = [=, &dest, &src] {
            LOGI("RGB8 work T3");
            size_t index = 2;
            process(index, (isARGBEnable() + 2));
        };

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
