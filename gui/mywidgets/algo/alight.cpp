#include "alight.h"
#include "defines.h"
#include "tools.h"

using namespace tools;

static UCHAR LOOKUPTABLE[256];
void lightTable(const int &weight = 125)
{
    //log曲线
    [=] {
        float brightness = float(weight) / 125.0;
        for (int i = 0; i < 256; i++)
        {
            LOOKUPTABLE[i] = (UCHAR)min(255, max(0, int(255 * log10((brightness * i + 25.5) / 255) + 255)));
        }
    }();

    // [=] {
    //     float brightness = float(weight);
    //     for (int i = 0; i < 256; i++)
    //     {
    //         LOOKUPTABLE[i] = (UCHAR)min(255.0, max(0.0, i + sin(PI * i / 255.0) * 70));
    //     }
    // }();

    //半圆函数, 压制高亮, 提亮暗区
    // [=] {
    //     for (int i = 0; i < 128; i++)
    //     {
    //         LOOKUPTABLE[i] = (UCHAR)sqrt(pow(128, 2) - pow((i - 128), 2));
    //     }
    //     for (int i = 128; i < 256; i++)
    //     {
    //         LOOKUPTABLE[i] = 256 - (UCHAR)sqrt(pow(128, 2) - pow((i - 128), 2));
    //     }
    // }();
}

QImage adjustBrightness(const QImage &img, const int &weight = 125)
{
    lightTable(weight);
    size_t width = img.width();
    size_t height = img.height();
    QImage out(img.size(), img.format());
    static const unsigned int thread_counts = ThreadPool::max_cpu();

    std::function<void(void)> process[thread_counts];
    std::future<void> result[thread_counts];
    for (auto i = 0; i < thread_counts; i++)
    {
        process[i] = [=, &out]() {
            LOGI("AutoLight work T[{}]", i);
            //正常不会溢出
            for (size_t x = width * i / thread_counts; x < width * (i + 1) / thread_counts; x++)
            {
                for (size_t y = 0; y < height; y++)
                {
                    QRgb rgb = img.pixel(x, y);
                    RGB<> _rgb = RGB<>(qRed(rgb), qGreen(rgb), qBlue(rgb));
                    HSV<> hsv = RGB2HSVTable(_rgb);
                    hsv._V = LOOKUPTABLE[hsv._V];

                    _rgb = HSV2RGBTable(hsv);
                    // out.setPixelColor(x, y, QColor(qRgb((int)_rgb.R, (int)_rgb.G, (int)_rgb.B)));
                    out.setPixel(x, y, uint((0xFF << 24) | (_rgb.R << 16) | (_rgb.G << 8) | (_rgb.B)));
                }
            }
        };
        result[i] = Thread::getThreadInstance()->enqueue(process[i]);
    }

    for (auto i = 0; i < thread_counts; i++)
    {
        result[i].get();
    }

    return out;
}