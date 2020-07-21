#include "mtask.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

mtask_t example_task1, example_task2;
static int example_entry(mtask_t *_task)
{
    printf("example_entry: %02d-%02d %02d:%02d:%02d\n", 
    _task->exec_time.mon, _task->exec_time.day, _task->exec_time.hour, 
    _task->exec_time.min, _task->exec_time.sec);
    printf("example_entry: tid = %d\n", _task->tid);
    printf("example_entry: status = %d\n", _task->status);
    if(_task->crontab != NULL)
        printf("example_entry: crontab = \"%s\"\n", _task->crontab);
    else 
        printf("example_entry: crontab = NULL\n");
    if(_task == &example_task1)
    {
        if(_task->exec_time.sec % 2 == 0) mtask_active_task(example_task2.tid);
    }
    return 0;
}

int main(void)
{
    // mtask_time_t example_time;
    // unsigned int example_tick;
    mtask_register(&example_task1, "0 */2 * * * *", example_entry);
    mtask_register(&example_task2, NULL, example_entry);
    // example_tick = mtask_ticks_read();
    while(1)
    {
        mtask_loop();
        // if(mtask_ticks_elapsed(&example_tick) >= 1000)
        // {
        //     example_tick = mtask_ticks_read();
        //     mtask_localtime_read(&example_time);
        //     printf("example_entry: %02d-%02d %02d:%02d:%02d\n", 
        //     example_time.mon, example_time.day, example_time.hour, 
        //     example_time.min, example_time.sec);
        // }
    }
}

