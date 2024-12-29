#ifndef __STATS_H__
#define __STATS_H__

#include "threadpool.h"
#include "defines.h"
#include "table.h"

#include <QImageReader>

typedef struct _STATS_INFO_{
    float ave_y;
}STATS_INFO;

void getStats(STATS_INFO&, const QImage&);

#endif