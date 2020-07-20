#include "mtask.h"
#include <time.h>

__weak int mtask_localtime(mtask_time_t *_time)
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

__weak unsigned int mtask_tick(void)
{

}

unsigned int mtask_tick_elapsed(unsigned int *ts)
{
    unsigned int cur_tick;
    cur_tick = mtask_tick();
    if(cur_tick < *ts)
        return 0xFFFFFFFF - *ts + 1 + cur_tick;
    return cur_tick - *ts;
}
