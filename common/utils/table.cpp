#include "table.h"
#include "tools.h"

#undef __LOGTAG__
#define __LOGTAG__ "TABLE"
#include "log.h"

#include <iostream>
#include <cmath>

static UCHAR TO8TABLE[static_cast<UINT32>(BITS::BITS_MAX)][65536] = {0};
static RGB<> YUV2RGBTABLE[256][256][256] = {RGB<>()};
static RGB<> HSV2RGBTABLE[256][256][256] = {RGB<>()};
static HSV<> RGB2HSVTABLE[256][256][256] = {HSV<>()};

const UCHAR &To8Table(const UINT32 &data, const BITS &mask)
{
    return TO8TABLE[static_cast<UINT32>(mask)][data];
}

const RGB<> &YUV2RGBTable(const YUV<> &data)
{
    return YUV2RGBTABLE[data.Y][data.U][data.V];
}

const RGB<> &HSV2RGBTable(const HSV<> &data)
{
    return HSV2RGBTABLE[data.H][data.S][data._V];
}

const HSV<> &RGB2HSVTable(const RGB<> &data)
{
    return RGB2HSVTABLE[data.R][data.G][data.B];
}

template <typename T>
inline T round(const T &min, const T &max, const T &inout)
{
    return (inout < min) ? min : ((inout > max) ? max : inout);
}

static VOID initTo8Table()
{
    float gamma = 1.25f;
    for (UINT32 bit = 0; bit < static_cast<UINT32>(BITS::BITS_MAX); bit++)
    {
        UCHAR bits = 10 + (bit << 1);
        UINT32 max_min = 1 << bits;
        // UINT32 max_min_h = max_min >> 1;
        for (UINT32 in = 1; in < 65536; in++)
        {
            // TO8TABLE[bit][in] = UCHAR(255 * in / max_min);
            // TO8TABLE[bit][in] = UCHAR(pow(2, 8 * log10(in) / (log10(max_min))));
            TO8TABLE[bit][in] = UCHAR(256 * pow(((float)in / max_min), ((float)1 / gamma)));
        }
    }
}

static VOID initYUV2RGBTable()
{
    for (size_t Y = 0; Y < 256; Y++)
    {
        for (size_t U = 0; U < 256; U++)
        {
            for (size_t V = 0; V < 256; V++)
            {
                int R = Y + ((359 * (V - 128)) >> 8);
                int G = Y - (((88 * (U - 128) + 183 * (V - 128))) >> 8);
                int B = Y + ((454 * (U - 128)) >> 8);
                YUV2RGBTABLE[Y][U][V] = RGB<>((UCHAR)round(0, 255, R), (UCHAR)round(0, 255, G), (UCHAR)round(0, 255, B));
            }
        }
    }
}

static VOID initRGB2HSVTable()
{
    for (size_t R = 0; R < 256; R++)
    {
        for (size_t G = 0; G < 256; G++)
        {
            for (size_t B = 0; B < 256; B++)
            {
                float R_ = R / 255.0;
                float G_ = G / 255.0;
                float B_ = B / 255.0;
                float C_max = tools::max(R_, G_, B_);
                float C_min = tools::min(R_, G_, B_);
                float delta = C_max - C_min;

                int H = [=]() -> UCHAR {
                    if (IS_ZERO(delta))
                    {
                        return 0;
                    }
                    if (C_max == R_)
                    {
                        return 60 * ((int((G_ - B_) / delta)) % 6) * 255 / 360;
                    }
                    if (C_max == G_)
                    {
                        return 60 * (int((B_ - R_) / delta) + 2) * 255 / 360;
                    }
                    if (C_max == B_)
                    {
                        return 60 * (int((R_ - G_) / delta) + 4) * 255 / 360;
                    }
                }();

                int S = [=]() -> UCHAR {
                    if (IS_ZERO(delta))
                    {
                        return 0;
                    }

                    return (delta / C_max) * 255;
                }();

                int V = [=]() -> UCHAR {
                    return C_max * 255;
                }();

                RGB2HSVTABLE[R][G][B] = HSV<>((UCHAR)round(0, 255, H), (UCHAR)round(0, 255, S), (UCHAR)round(0, 255, V));

                // if ((R % 10 == 0) && (G % 10 == 0) && (B % 10 == 0))
                // {
                //     std::cout << HSV<int>(RGB2HSVTABLE[R][G][B]);
                // }
            }
        }
    }
}

static VOID initHSV2RGBTable()
{
    for (size_t H = 0; H < 256; H++)
    {
        for (size_t S = 0; S < 256; S++)
        {
            for (size_t V = 0; V < 256; V++)
            {
                float H_ = H * 360.0 / 255.0;
                float S_ = S / 255.0;
                float V_ = V / 255.0;

                float C = V_ * S_;
                float X = C * (1 - abs((int(H_) / 60) % 2 - 1));
                float m = V_ - C;

                RGB<float> rgb = [=]() -> RGB<float> {
                    if (0 <= H_ && H_ < 60)
                    {
                        return RGB<float>(C, X, 0);
                    }
                    if (60 <= H_ && H_ < 120)
                    {
                        return RGB<float>(X, C, 0);
                    }
                    if (120 <= H_ && H_ < 180)
                    {
                        return RGB<float>(0, C, X);
                    }
                    if (180 <= H_ && H_ < 240)
                    {
                        return RGB<float>(0, X, C);
                    }
                    if (240 <= H_ && H_ < 300)
                    {
                        return RGB<float>(X, 0, C);
                    }
                    if (300 <= H_ && H_ <= 360)
                    {
                        return RGB<float>(C, 0, X);
                    }
                }();

                HSV2RGBTABLE[H][S][V] = RGB<>((UCHAR)round(0, 255, (int)((rgb.R + m) * 255)),
                                              (UCHAR)round(0, 255, (int)((rgb.G + m) * 255)),
                                              (UCHAR)round(0, 255, (int)((rgb.B + m) * 255)));

                // if ((H % 100 == 0) && (S % 100 == 0) && (V % 100 == 0))
                // {
                //     std::cout << "out: " << RGB<int>(RGB2HSVTABLE[H][S][V]) << std::endl;
                //     std::cout << "rgb: " << rgb << std::endl;
                //     std::cout << "___: " << int(H_) << ", " << S_ << ", " << V_ << std::endl;
                //     std::cout << "cxm: " << C << ", " << X << ", " << m << std::endl;
                // }
            }
        }
    }
}

static VOID initAllTable()
{
    initTo8Table();
    // LOGI("init initTo8Table");
    initYUV2RGBTable();
    // LOGI("init initYUV2RGBTable");
    initHSV2RGBTable();
    // LOGI("init initHSV2RGBTable");
    initRGB2HSVTable();
    // LOGI("init initRGB2HSVTable");
}

class InitTable
{
public:
    InitTable()
    {
        initAllTable();
    }
};
const static InitTable INITTABLE;