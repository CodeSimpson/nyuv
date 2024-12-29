#ifndef __ALIGHT_H__
#define __ALIGHT_H__

#include "threadpool.h"
#include "defines.h"
#include "table.h"
#include "tools.h"

#include <QImageReader>

void lightTable(const int &);
QImage adjustBrightness(const QImage &, const int &);

#endif