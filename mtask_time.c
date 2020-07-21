#include "mtask.h"
#include <time.h>

int mtask_localtime_read(mtask_time_t *_time)
{
    time_t rawtime;
    struct tm *info;
    rawtime = time(NULL);
    info = localtime(&rawtime);
    _time->sec = info->tm_sec;
    _time->min = info->tm_min;
    _time->hour = info->tm_hour;
    _time->day = info->tm_mday;
    _time->mon = info->tm_mon+1;
    _time->week = info->tm_wday;
    return 0;
}

unsigned int mtask_ticks_read(void)
{
    unsigned int ticks;
    ticks = clock() / CLOCKS_PER_SEC * 1000;
    ticks &= 0xFFFFFFFF;
    return ticks;
}

unsigned int mtask_ticks_elapsed(unsigned int *ts)
{
    unsigned int cur_tick;
    cur_tick = mtask_ticks_read();
    if(cur_tick < *ts)
        return 0xFFFFFFFF - *ts + 1 + cur_tick;
    return cur_tick - *ts;
}
