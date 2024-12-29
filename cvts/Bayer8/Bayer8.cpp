#include "Bayer8.h"
#include "threadpool.h"

#undef __LOGTAG__
#define __LOGTAG__ "Bayer8"
#include "log.h"

using namespace std;

namespace cvt
{

    Bayer8::Bayer8(const STR &type)
    {
        // LOGI("Bayer8 start.");

        if (type == "Bayer8GRBG")
        {
            m_cvtypeout = BAYER8TYPE::GRBG;
        }
        else if (type == "Bayer8RGGB")
        {
            m_cvtypeout = BAYER8TYPE::RGGB;
        }
        else if (type == "Bayer8GBRG")
        {
            m_cvtypeout = BAYER8TYPE::GBRG;
        }
        else if (type == "Bayer8BGGR")
        {
            m_cvtypeout = BAYER8TYPE::BGGR;
        }
    }

    Bayer8::~Bayer8()
    {
        // LOGI("Bayer8 done.");
    }

    VOID Bayer8::setOBufferInfo(FORMAT &oformat, const FORMAT &iformat)
    {
        oformat.width = iformat.width;
        oformat.height = iformat.height;
        oformat.stride = oformat.width;

        //RGB888, w * h * 3
        oformat.size = oformat.height * oformat.width * 3;
    }

    VOID Bayer8::getIBufferInfo(FORMAT &iformat)
    {
        size_t size_wh = iformat.height * iformat.width;
        size_t size_sh = iformat.height * iformat.stride;

        iformat.size = max(size_wh, size_sh);
    }

    RESULT Bayer8::cvt(REQUESTINFO &requestInfo)
    {
        size_t required_size = requestInfo.iformat.height * requestInfo.iformat.width;
        if (required_size > requestInfo.ibuffer.size())
        {
            LOGE("required size {}, but load {}", required_size, requestInfo.ibuffer.size());
            return RESULT::INVALID_FORMAT;
        }

        m_format = requestInfo.iformat;
        m_bp = requestInfo.bayer;

#ifdef BAYER_PROCESS_V1
        processV1(requestInfo.obuffer, requestInfo.ibuffer);
#endif

#ifdef BAYER_PROCESS_V2
        processV2(requestInfo.obuffer, requestInfo.ibuffer);
#endif

        return RESULT::SUCCESS;
    }

#ifdef BAYER_PROCESS_V1
    VOID Bayer8::processV1(BUFFER &dest, BUFFER &src)
    {
        bool bp = m_bp;

        //width, height 一定是偶数
        size_t width = m_format.width;
        size_t height = m_format.height;

        //P1P2P1P2P1P2P1P2
        //P3P4P3P4P3P4P3P4
        //P1P2P1P2P1P2P1P2
        //P3P4P3P4P3P4P3P4
        //P1P2P1P2P1P2P1P2
        //P3P4P3P4P3P4P3P4
        //P1P2P1P2P1P2P1P2
        //P3P4P3P4P3P4P3P4
        using BP = std::function<void(size_t &, size_t &, size_t &)>;
        BP P1, P2, P3, P4;
        //以通道为起点，逆时针

        BP GRG = [=, &dest, &src]() -> BP {
            if (!bp)
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = (src[y * width + x - 1] + src[y * width + x + 1]) >> 1; //R
                        dest[index + 1] = src[y * width + x];
                        // dest[index+1] = src[y * width + x] >> 1 +
                        //                 (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y - 1) * width + x + 1] + src[(y + 1) * width + x - 1]) >> 3;                                         //G
                        dest[index + 2] = (src[(y - 1) * width + x] + src[(y + 1) * width + x]) >> 1; //B
                    };
            }
            else
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = 0; //R
                        dest[index + 1] = src[y * width + x];
                        dest[index + 2] = 0; //B
                    };
            }
        }();
        BP GBG = [=, &dest, &src]() -> BP {
            if (!bp)
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = (src[(y - 1) * width + x] + src[(y + 1) * width + x]) >> 1; //R
                        dest[index + 1] = src[y * width + x];
                        // dest[index+1] = src[y * width + x] >> 1 +
                        //                 (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y - 1) * width + x + 1] + src[(y + 1) * width + x - 1]) >> 3;
                        dest[index + 2] = (src[y * width + x - 1] + src[y * width + x + 1]) >> 1; //B
                    };
            }
            else
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = 0; //R
                        dest[index + 1] = src[y * width + x];
                        dest[index + 2] = 0; //B
                    };
            }
        }();
        BP RGB = [=, &dest, &src]() -> BP {
            if (!bp)
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = src[y * width + x];                                                                                                                   //R
                        dest[index + 1] = (src[(y - 1) * width + x] + src[(y + 1) * width + x] + src[y * width + x - 1] + src[y * width + x + 1]) >> 2;                     //G
                        dest[index + 2] = (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y - 1) * width + x + 1] + src[(y + 1) * width + x - 1]) >> 2; //B
                    };
            }
            else
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = src[y * width + x]; //R
                        dest[index + 1] = 0;
                        dest[index + 2] = 0; //B
                    };
            }
        }();
        BP BGR = [=, &dest, &src]() -> BP {
            if (!bp)
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y - 1) * width + x + 1] + src[(y + 1) * width + x - 1]) >> 2; //R
                        dest[index + 1] = (src[(y - 1) * width + x] + src[(y + 1) * width + x] + src[y * width + x - 1] + src[y * width + x + 1]) >> 2;                 //G
                        dest[index + 2] = src[y * width + x];
                    };
            }
            else
            {
                return
                    [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
                        index = (x + y * width) * 3;
                        dest[index] = 0; //R
                        dest[index + 1] = 0;
                        dest[index + 2] = src[y * width + x]; //B
                    };
            }
        }();
        auto ZERO = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3;
            dest[index++] = 0; //R
            dest[index++] = 0; //G
            dest[index++] = 0; //B
        };

        switch (m_cvtypeout)
        {
        default:
        case BAYER8TYPE::BGGR:
            P1 = BGR, P2 = GBG, P3 = GRG, P4 = RGB;
            break;
        case BAYER8TYPE::GBRG:
            P1 = GBG, P2 = BGR, P3 = RGB, P4 = GRG;
            break;
        case BAYER8TYPE::GRBG:
            P1 = GRG, P2 = RGB, P3 = BGR, P4 = GBG;
            break;
        case BAYER8TYPE::RGGB:
            P1 = RGB, P2 = GRG, P3 = GBG, P4 = BGR;
            break;
        }

        //参考以上P1-P4的排列，决定P1-P4的起点和终点，每个点x，y的跨度都是2
        auto processP1 = [=, &dest, &src] {
            LOGI("BAYER8 work T1");
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P1(index, x, y);
                }
            }
        };
        auto processP2 = [=, &dest, &src] {
            LOGI("BAYER8 work T2");
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P2(index, x, y);
                }
            }
        };
        auto processP3 = [=, &dest, &src] {
            LOGI("BAYER8 work T3");
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P3(index, x, y);
                }
            }
        };
        auto processP4 = [=, &dest, &src] {
            LOGI("BAYER8 work T4");
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P4(index, x, y);
                }
            }
        };
        auto processPboder = [=, &dest, &src] {
            LOGI("BAYER8 work T0");
            size_t index = 0;
            for (size_t x = 0, y = 0; x < width; x++)
            {
                ZERO(index, x, y);
            }
            for (size_t x = 0, y = height - 1; x < width; x++)
            {
                ZERO(index, x, y);
            }
            for (size_t x = 0, y = 0; y < height; y++)
            {
                ZERO(index, x, y);
            }
            for (size_t x = width - 1, y = 0; y < height; y++)
            {
                ZERO(index, x, y);
            }
        };

        //并行耗时约120ms，优化for循环后可以到50ms左右！
        auto resultP1 = Thread::getThreadInstance()->enqueue(processP1);
        auto resultP2 = Thread::getThreadInstance()->enqueue(processP2);
        auto resultP3 = Thread::getThreadInstance()->enqueue(processP3);
        auto resultP4 = Thread::getThreadInstance()->enqueue(processP4);
        // auto resultPborder = Thread::getThreadInstance()->enqueue(processPboder);
        //这个计算量比较小,可以不需要并行化
        processPboder();

        //wait
        resultP1.get();
        resultP2.get();
        resultP3.get();
        resultP4.get();
    }
#endif
#ifdef BAYER_PROCESS_V2
    VOID Bayer8::processV2G(BUFFER &dest, BUFFER &src)
    {
        //width, height 一定是偶数
        size_t width = m_format.width;
        size_t height = m_format.height;

        //以通道为起点，逆时针, 回复G
        auto GRG = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3 + 1;
            dest[index] = src[y * width + x];
        };
        auto GBG = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3 + 1;
            dest[index] = src[y * width + x];
        };
        auto RGB = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3 + 1;
            dest[index] = (src[(y - 1) * width + x] + src[(y + 1) * width + x] + src[y * width + x - 1] + src[y * width + x + 1]) >> 2; //G
        };
        auto BGR = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3 + 1;
            dest[index] = (src[(y - 1) * width + x] + src[(y + 1) * width + x] + src[y * width + x - 1] + src[y * width + x + 1]) >> 2; //G
        };

        std::function<void(size_t &, size_t &, size_t &)> P1, P2, P3, P4;
        switch (m_cvtypeout)
        {
        default:
        case BAYER8TYPE::BGGR:
            P1 = BGR, P2 = GBG, P3 = GRG, P4 = RGB;
            break;
        case BAYER8TYPE::GBRG:
            P1 = GBG, P2 = BGR, P3 = RGB, P4 = GRG;
            break;
        case BAYER8TYPE::GRBG:
            P1 = GRG, P2 = RGB, P3 = BGR, P4 = GBG;
            break;
        case BAYER8TYPE::RGGB:
            P1 = RGB, P2 = GRG, P3 = GBG, P4 = BGR;
            break;
        }

        //参考以上P1-P4的排列，决定P1-P4的起点和终点，每个点x，y的跨度都是2
        auto processP1 = [=, &dest, &src] {
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P1(index, x, y);
                }
            }
        };
        auto processP2 = [=, &dest, &src] {
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P2(index, x, y);
                }
            }
        };
        auto processP3 = [=, &dest, &src] {
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P3(index, x, y);
                }
            }
        };
        auto processP4 = [=, &dest, &src] {
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P4(index, x, y);
                }
            }
        };

        auto resultP1 = Thread::getThreadInstance()->enqueue(processP1);
        auto resultP2 = Thread::getThreadInstance()->enqueue(processP2);
        auto resultP3 = Thread::getThreadInstance()->enqueue(processP3);
        auto resultP4 = Thread::getThreadInstance()->enqueue(processP4);
        resultP1.get();
        resultP2.get();
        resultP3.get();
        resultP4.get();
    }

    VOID Bayer8::processV2RB(BUFFER &dest, BUFFER &src)
    {
        //width, height 一定是偶数
        size_t width = m_format.width;
        size_t height = m_format.height;

        //以通道为起点，逆时针, 回复R
        auto GRG = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3;
            dest[index] = (src[y * width + x - 1] + src[y * width + x + 1]) >> 1 + (2 * dest[index + 1] - dest[index - 2] - dest[index + 4]) >> 1;
            dest[index + 2] = (src[(y - 1) * width + x] + src[(y + 1) * width + x]) >> 1 + (2 * dest[index + 1] - dest[index - 3 * width + 1] - dest[index + 3 * width + 1]) >> 1;
        };
        auto GBG = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3;
            dest[index] = (src[(y - 1) * width + x] + src[(y + 1) * width + x]) >> 1 + (2 * dest[index + 1] - dest[index - 3 * width + 1] - dest[index + 3 * width + 1]) >> 1;
            dest[index + 2] = (src[y * width + x - 1] + src[y * width + x + 1]) >> 1 + (2 * dest[index + 1] - dest[index - 2] - dest[index + 4]) >> 1;
        };
        auto RGB = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3;
            dest[index] = src[y * width + x];
            dest[index + 2] = (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y + 1) * width + x - 1] + src[(y - 1) * width + x + 1]) >> 2 + (4 * dest[index + 1] - dest[index - 3 * width - 2] - dest[index - 3 * width + 4] - dest[index + 3 * width - 2] - dest[index + 3 * width + 4]) >> 2;
        };
        auto BGR = [=, &dest, &src](size_t &index, size_t &x, size_t &y) -> void {
            index = (x + y * width) * 3;
            dest[index] = (src[(y - 1) * width + x - 1] + src[(y + 1) * width + x + 1] + src[(y + 1) * width + x - 1] + src[(y - 1) * width + x + 1]) >> 2 + (4 * dest[index + 1] - dest[index - 3 * width - 2] - dest[index - 3 * width + 4] - dest[index + 3 * width - 2] - dest[index + 3 * width + 4]) >> 2;
            dest[index + 2] = src[y * width + x];
        };

        std::function<void(size_t &, size_t &, size_t &)> P1, P2, P3, P4;
        switch (m_cvtypeout)
        {
        default:
        case BAYER8TYPE::BGGR:
            P1 = BGR, P2 = GBG, P3 = GRG, P4 = RGB;
            break;
        case BAYER8TYPE::GBRG:
            P1 = GBG, P2 = BGR, P3 = RGB, P4 = GRG;
            break;
        case BAYER8TYPE::GRBG:
            P1 = GRG, P2 = RGB, P3 = BGR, P4 = GBG;
            break;
        case BAYER8TYPE::RGGB:
            P1 = RGB, P2 = GRG, P3 = GBG, P4 = BGR;
            break;
        }

        //参考以上P1-P4的排列，决定P1-P4的起点和终点，每个点x，y的跨度都是2
        auto processP1 = [=, &dest, &src] {
            LOGI("BAYER8 work T1");
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P1(index, x, y);
                }
            }
        };
        auto processP2 = [=, &dest, &src] {
            LOGI("BAYER8 work T2");
            size_t index = 0;
            for (size_t y = 2; y <= height - 2; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P2(index, x, y);
                }
            }
        };
        auto processP3 = [=, &dest, &src] {
            LOGI("BAYER8 work T3");
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 2; x <= width - 2; x += 2)
                {
                    P3(index, x, y);
                }
            }
        };
        auto processP4 = [=, &dest, &src] {
            LOGI("BAYER8 work T4");
            size_t index = 0;
            for (size_t y = 1; y <= height - 3; y += 2)
            {
                for (size_t x = 1; x <= width - 3; x += 2)
                {
                    P4(index, x, y);
                }
            }
        };

        auto resultP1 = Thread::getThreadInstance()->enqueue(processP1);
        auto resultP2 = Thread::getThreadInstance()->enqueue(processP2);
        auto resultP3 = Thread::getThreadInstance()->enqueue(processP3);
        auto resultP4 = Thread::getThreadInstance()->enqueue(processP4);
        resultP1.get();
        resultP2.get();
        resultP3.get();
        resultP4.get();
    }

    VOID Bayer8::processV2(BUFFER &dest, BUFFER &src)
    {
        processV2G(dest, src);
        processV2RB(dest, src);
    }
#endif
} // namespace cvt