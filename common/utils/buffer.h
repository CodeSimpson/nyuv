#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <iostream>
#include <algorithm>

#undef __LOGTAG__
#define __LOGTAG__ "BUFFER"
#include "log.h"

typedef int INT32;
typedef unsigned int UINT32;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef bool BOOL;
typedef void VOID;
typedef float FLOAT;
typedef unsigned short USHORT;

/***
 * 图像在内存中的buffer，地址和大小
 * 目的是：如果是复制操作、则内存地址不变，例如
 * BUFFER A
 * BUFFER B = A
 * 以上A、B共享同一块内存，当且仅当A、B都被析构时内存才会被释放，类似于shared_ptr
 * 内部使用一个引用计数器实现。
 * **/

//TODO：应考虑多线程优化，就nyuv来说，性能还是有点差的
typedef class __BUFFER__
{
public:
    //自我管理
    __BUFFER__()
    {
        mp_buffer = NULL;
        mp_size = NULL;
        mp_refcounts = NULL;

        mp_buffer_copy = NULL;
        mp_size_copy = NULL;
        mp_refcounts_copy = NULL;
    }
    ~__BUFFER__()
    {
        if (refdd())
        {
            deleteAll();
        }
    }

    //赋值操作，共享内存
    VOID operator=(__BUFFER__ &instance)
    {
        deleteAll();
        //需要记住右边buffer的内存地址
        mp_buffer_copy = &(instance.mp_buffer);
        mp_size_copy = &(instance.mp_size);
        mp_refcounts_copy = &(instance.mp_refcounts);

        mp_buffer = instance.mp_buffer;
        mp_size = instance.mp_size;
        mp_refcounts = instance.mp_refcounts;
        refpp();
    }
    UCHAR &operator[](int index) const
    {
        return mp_buffer[index];
    }

    //resize操作实际上是分配内存，会将自身内存清除
    VOID resize(const size_t &buffer_size)
    {
        //checking
        if (mp_size == NULL)
        {
            mp_size = new size_t(0);
        }
        if (mp_refcounts == NULL)
        {
            mp_refcounts = new size_t(1);
        }

        bool need_new = true;
        if (mp_buffer)
        {
            if ((*mp_size) >= buffer_size)
            {
                need_new = false;
            }
            else
            {
                LOGI("delete buffer obj={} size={} refs={} bup={}", (void *)this, *mp_size, *mp_refcounts, (void *)mp_buffer);
                delete[] mp_buffer;
                mp_buffer = NULL;
            }
        }

        //allocating
        *mp_size = buffer_size;
        *mp_refcounts = 1;
        if (need_new)
            mp_buffer = new UCHAR[*mp_size];

        //虽然清除了自身内存，但是会同步更新
        if (mp_buffer_copy != NULL)
            *mp_buffer_copy = mp_buffer;
        if (mp_size_copy != NULL)
            *mp_size_copy = mp_size;
        if (mp_refcounts_copy != NULL)
            *mp_refcounts_copy = mp_refcounts;

        LOGI("resize buffer obj={} size={} refs={} bup={}", (void *)this, *mp_size, *mp_refcounts, (void *)mp_buffer);
    }

    //只是单纯的值交换
    VOID swap(__BUFFER__ &instance)
    {
        std::swap(mp_buffer, instance.mp_buffer);
        std::swap(mp_size, instance.mp_size);
        std::swap(mp_refcounts, instance.mp_refcounts);
        std::swap(mp_buffer_copy, instance.mp_buffer_copy);
        std::swap(mp_size_copy, instance.mp_size_copy);
        std::swap(mp_refcounts_copy, instance.mp_refcounts_copy);
    }

    UCHAR *begin() const
    {
        return mp_buffer;
    }

    UCHAR *end() const
    {
        if ((mp_size != NULL) && ((*mp_size) > 0))
            return &mp_buffer[(*mp_size) - 1];
        else
            return mp_buffer;
    }

    BOOL empty() const
    {
        return !mp_buffer;
    }

    const size_t size() const
    {
        if (mp_size == NULL)
            return 0;
        return *mp_size;
    }

private:
    BOOL refpp()
    {
        if (mp_refcounts)
        {
            (*mp_refcounts)++;
            LOGI("ref buffer obj={} size={} refs={} bup={}", (void *)this, *mp_size, *mp_refcounts, (void *)mp_buffer);
        }

        return true;
    }

    BOOL refdd()
    {
        bool ret = false;
        if (mp_refcounts)
        {
            if ((*mp_refcounts) > 0)
            {
                (*mp_refcounts)--;
            }
            if ((*mp_refcounts) <= 0)
            {
                (*mp_refcounts) = 0;
                ret = true;
            }
            LOGI("ref buffer obj={} size={} refs={} bup={}", (void *)this, *mp_size, *mp_refcounts, (void *)mp_buffer);
        }

        return ret;
    }

    VOID deleteAll()
    {
        LOGI("delete buffer obj={} size={} refs={} bup={}", (void *)this, *mp_size, *mp_refcounts, (void *)mp_buffer);
        if (mp_size)
        {
            delete mp_size;
            mp_size = NULL;
        }
        if (mp_refcounts)
        {
            delete mp_refcounts;
            mp_refcounts = NULL;
        }
        if (mp_buffer)
        {
            delete[] mp_buffer;
            mp_buffer = NULL;
        }

        //这里很重要，释放内存的时候，要把原始内存置空
        if (mp_buffer_copy != NULL)
            *mp_buffer_copy = NULL;
        if (mp_size_copy != NULL)
            *mp_size_copy = NULL;
        if (mp_refcounts_copy != NULL)
            *mp_refcounts_copy = NULL;
        mp_buffer_copy = NULL;
        mp_size_copy = NULL;
        mp_refcounts_copy = NULL;
    }

private:
    UCHAR *mp_buffer;
    size_t *mp_size;
    size_t *mp_refcounts;

    //用于指向原始数据，是指向原始数据内存地址的指针
    UCHAR **mp_buffer_copy;
    size_t **mp_size_copy;
    size_t **mp_refcounts_copy;
} BUFFER;

#endif