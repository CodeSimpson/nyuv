#include <atomic>
#include "stats.h"
#undef __LOGTAG__
#define __LOGTAG__ "STATS"
#include "log.h"

STATS_INFO calStats(const QImage &img);
void getStats(STATS_INFO& stats, const QImage& img)
{
    stats = calStats(img);
}

STATS_INFO calStats(const QImage &img)
{
    STATS_INFO result;

    std::atomic<double> v_sum;
    v_sum = 0;

    size_t width = img.width();
    size_t height = img.height();
    static const unsigned int thread_counts = ThreadPool::max_cpu();

    std::function<void(void)> process[thread_counts];
    std::future<void> thread_result[thread_counts];
    for (auto i = 0; i < thread_counts; i++)
    {
        process[i] = [&, i]() {
            //正常不会溢出
            double v_sum_i = 0;
            for (size_t x = width * i / thread_counts; (x < width) && (x < width * (i + 1) / thread_counts); x++)
            {
                for (size_t y = 0; y < height; y++)
                {
                    QRgb rgb = img.pixel(x, y);
                    RGB<> _rgb = RGB<>(qRed(rgb), qGreen(rgb), qBlue(rgb));
                    HSV<> hsv = RGB2HSVTable(_rgb);
                    v_sum_i += hsv.V;
                }
            }
            v_sum = v_sum + v_sum_i;
            LOGI("CalStats work T[{}] v_sum_i {}", i, v_sum_i);
        };
        thread_result[i] = Thread::getThreadInstance()->enqueue(process[i]);
    }

    for (auto i = 0; i < thread_counts; i++)
    {
        thread_result[i].get();
    }

    LOGI("current image size {}x{}, v_sum {}", width, height, v_sum);
    {
        result.ave_y = v_sum / (width * height);
    }

    return result;
}