#include "npea.h"

typedef const QImage &QI;
typedef const size_t &PP;
typedef const UCHAR &PV;

#define IS_VALID_POS(img, x, y) (((x) >= 0 && (x) < (img).width()) && ((y) >= 0 && (y) < (img).height()))
#define IS_VALID_VAL(v) ((0 <= (v)) && ((v) < 255))

static UCHAR Lxy(QI img, PP x, PP y)
{
    QRgb _rgb = img.pixel(x, y);
    RGB<> rgb = RGB<>(qRed(_rgb), qGreen(_rgb), qBlue(_rgb));
    return rgb.max();
}

static bool Uxy(PP x, PP y)
{
    return (x >= y);
}

static int Gxy(QI img, PP i, PP j)
{
    return IS_VALID_POS(img, i, j) ? Lxy(img, i, j) : -1;
}

//对于在位置(x,y)的值k，NN_{k, l}(x, y)表示领域内值为l的数量
static size_t NNklxy(QI img, PV l, PP x, PP y)
{
    PV k = img.pixel(x, y);
    auto Gxy_p = [=](PP i, PP j) -> int { return Gxy(img, i, j); };
    int NBxy[] = {Gxy_p(x, y - 1),
                  Gxy_p(x - 1, y), Gxy_p(x, y), Gxy_p(x + 1, y),
                  Gxy_p(x, y + 1)};

    size_t result = 0;
    for (auto &g : NBxy)
    {
        if (g == l)
            result++;
    }

    return result;
}

static size_t Qkl(QI img, PV k, PV l, PV m = 1)
{
    if (IS_VALID_VAL(k) && IS_VALID_VAL(l))
    {
        return 0;
    }
    PP w = img.width();
    PP h = img.height();
    auto Gxy_p = [=](PP i, PP j) -> int { return Gxy(img, i, j); };

    size_t result = 0;
    PV lp_min = l - m;
    PV lp_max = l + m;
    for (UCHAR lp = lp_min; lp <= lp_max; lp++)
    {
        for (size_t x = 0; x < w; x++)
        {
            for (size_t y = 0; y < h; y++)
            {
                if (Gxy_p(x, y) == k)
                    result += NNklxy(img, lp, x, y);
            }
        }
    }
    return result / (2 * m + 1);
}

static size_t patch_size = 15;
static size_t patch_ratio = patch_size / 2;

template <typename F>
static size_t patch(QI img, PP x, PP y, F func)
{
    size_t result = 0;
    for (size_t i = x - patch_ratio; i <= (x + patch_ratio); i++)
    {
        for (size_t j = y - patch_ratio; j <= (y + patch_ratio); j++)
        {
            if (IS_VALID_POS(img, x, y) && IS_VALID_POS(img, i, j))
                result += func(i, j);
        }
    }
    return result;
}

static size_t Wxy(QI img, PP x, PP y)
{
    auto Gxy_p = [=](PP i, PP j) -> int { return Gxy(img, i, j); };
    auto Qkl_p = [=](PP k, PP l) -> int { return Qkl(img, k, k); };

    auto func = [=](PP i, PP j) -> size_t {
        return Qkl_p(Gxy_p(x, y), Gxy_p(i, j)) * Uxy(Gxy_p(i, j), Gxy_p(x, y));
    };
    size_t result = patch(img, x, y, func);

    return result;
}

static size_t BPFGxy(QI img, PP x, PP y)
{
    auto Gxy_p = [=](PP i, PP j) -> int { return Gxy(img, i, j); };
    auto Qkl_p = [=](PP k, PP l) -> int { return Qkl(img, k, k); };
    
    auto func = [=](PP i, PP j) -> size_t {
        return Qkl_p(Gxy_p(x, y), Gxy_p(i, j)) * Uxy(Gxy_p(i, j), Gxy_p(x, y)) * Gxy_p(i, j);
    };
    size_t result = patch(img, x, y, func);

    return result / Wxy(img, x, y);
}

static size_t RGxy(QI img, PP x, PP y)
{
    PP w = img.width();
    PP h = img.height();
    auto Gxy_p = [=](PP i, PP j) -> int { return Gxy(img, i, j); };
    auto Lxy_p = [=](PP i, PP j) -> int { return Lxy(img, i, j); };
    auto Lexy_p = [=](PP i, PP j) -> int { return Lxy(img, i, j); };

    size_t result = 0;
    for (size_t i = 0; i < w; i++)
    {
        for (size_t j = 0; j < h; j++)
        {
            result += (Uxy(Lxy_p(x, y), Lxy_p(i, j)) ^ Uxy(Lexy_p(x, y), Lexy_p(i, j)));
        }
    }

    return result;
}

static size_t LOE(QI img)
{
    PP w = img.width();
    PP h = img.height();

    size_t result = 0;
    for (size_t i = 0; i < w; i++)
    {
        for (size_t j = 0; j < h; j++)
        {
            result += RGxy(img, i, j);
        }
    }

    return result / (w * h);
}
// static BPFxy()