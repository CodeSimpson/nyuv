#include "Gray8.h"
#include "threadpool.h"

#undef __LOGTAG__
#define __LOGTAG__ "Gray8"
#include "log.h"

using namespace std;

namespace cvt
{

    Gray8::Gray8(const STR &type)
    {
    }

    Gray8::~Gray8()
    {
        // LOGI("Gray8 done.");
    }

    VOID Gray8::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width;

        //RGB888, w * h * 3
        oformat.size = oformat.height * oformat.width * 3;
    }

    VOID Gray8::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    RESULT Gray8::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;
        
        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    VOID Gray8::process(BUFFER &dest, BUFFER &src)
    {
        //width, height 一定是偶数
        size_t width = m_format.width;
        size_t height = m_format.height;
        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                size_t index = h * m_format.stride + w;
                size_t dest_index = (h * width + w) * 3;
                dest[dest_index] = src[index];
                dest[dest_index + 1] = src[index];
                dest[dest_index + 2] = src[index]; 
            }
        }
    }

} // namespace cvt