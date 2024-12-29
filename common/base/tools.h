/*
 * @Author: your name
 * @Date: 2020-08-24 19:19:53
 * @LastEditTime: 2020-08-27 15:49:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /nyuv/common/base/tools.h
 */
#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <chrono>
#include <utility>

#define IS_ZERO(x) ((-0.0001 <= (x)) && ((x) <= 0.0001))

namespace tools
{
    template <typename RET, typename FUNC>
    inline std::pair<RET, double> runByTime(FUNC func)
    {
        auto start = std::chrono::steady_clock::now();

        auto ret = func();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        double cost = elapsed.count();

        return std::pair<decltype(ret), double>(ret, cost);
    }

    template <typename T>
    inline T max(const T &a)
    {
        return a;
    }
    template <typename T>
    inline T max(const T &a, const T &b)
    {
        return (a >= b) ? a : b;
    }
    template <typename T, typename... ARGS>
    inline T max(const T &t, ARGS... args)
    {
        return max(t, max(args...));
    }

    template <typename T>
    inline T min(const T &a)
    {
        return a;
    }
    template <typename T>
    inline T min(const T &a, const T &b)
    {
        return (a <= b) ? a : b;
    }
    template <typename T, typename... ARGS>
    inline T min(const T &t, ARGS... args)
    {
        return min(t, min(args...));
    }

    // template <typename T>
    // inline T pow(const T &t, const size_t &times){
    //     return (times > 1) ? t * pow(t, times - 1) : t;
    // }
} // namespace

#endif