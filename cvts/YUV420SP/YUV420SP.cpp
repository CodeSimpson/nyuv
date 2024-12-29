#include <cstring>

#include "YUV420SP.h"
#include "table.h"

#undef __LOGTAG__
#define __LOGTAG__ "YUV420SP"
#include "log.h"

using namespace std;

namespace cvt
{

    YUV420SP::YUV420SP(const STR &type)
    {
        // LOGI("YUV420SP start.");

        if (type == "YUV420SPNV12")
        {
            m_cvtypeout = YUV420SPTYPE::NV12;
        }
        else if (type == "YUV420SPNV21")
        {
            m_cvtypeout = YUV420SPTYPE::NV21;
        }
    }

    YUV420SP::~YUV420SP()
    {
        // LOGI("YUV420SP done.");
    }

    VOID YUV420SP::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width;

        //RGB888, w * h * 3
        oformat.size = oformat.height * oformat.width * 3;
    }

    VOID YUV420SP::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width * 3 / 2;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    RESULT YUV420SP::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width * 3 / 2;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;

        process(requestInfo.obuffer, requestInfo.ibuffer);

        return RESULT::SUCCESS;
    }

    VOID YUV420SP::process(BUFFER &dest, BUFFER &src)
    {
        int index = 0;
        size_t width = m_format.width;
        size_t height = m_format.height;

        UCHAR *ybase = src.begin();
        UCHAR *uvbase = &src[width * height];

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                //YYYYYYYYUVUV
                size_t uvbase_index = (y / 2 * width / 2 + (x / 2)) * 2;
                UCHAR Y = ybase[x + y * width];
                UCHAR U, V;

                switch (m_cvtypeout)
                {
                case YUV420SPTYPE::NV12:
                    U = uvbase[uvbase_index];
                    V = uvbase[uvbase_index + 1];
                    break;

                case YUV420SPTYPE::NV21:
                    U = uvbase[uvbase_index + 1];
                    V = uvbase[uvbase_index];
                    break;

                default:
                    U = uvbase[uvbase_index];
                    V = uvbase[uvbase_index + 1];
                    break;
                }

                RGB<> rgb = YUV2RGBTable(YUV<>(Y, U, V));
                dest[index++] = rgb.R;
                dest[index++] = rgb.G;
                dest[index++] = rgb.B;
            }
        }
    }

} //namespace cvt