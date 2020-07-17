#include "mtask.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
unsigned int time_cnt = 0;
mtask_t *mtask_head = NULL;

static mtask_t *active_task_read(void);

void mtask_microsecond_callback(void)
{
    time_cnt ++;
}

int mtask_elapsed(unsigned int *ts)
{
    if(time_cnt < *ts)
        return 0xFFFFFFFF - *ts + 1 + time_cnt;
    return time_cnt-*ts;
}

int mtask_loop(void)
{
    mtask_t *mtask_run;
    mtask_run = active_task_read();
    if(mtask_run != NULL)
    {
        mtask_run->entry(mtask_run);
    }
}

int mtask_active_task(const char *task_name)
{
    mtask_t *obj;
    obj = mtask_head;
    while(obj != NULL)
    {
        if(strcmp(obj->name, task_name) == 0)
        {
            if(obj->status != MTASK_IDLE) return -2;
            obj->status = MTASK_WAITING;
            return 0;
        }
        obj = obj->next;
    }
    return -1;
}

int mtask_register(mtask_t *target, int reload)
{
    mtask_t *obj;
    int cur_id;
    if(mtask_head == NULL) 
    {
        mtask_head = target;
        target->id = 1;
        target->status = MTASK_IDLE;
        target->next = NULL;
        return 0;
    }
    obj = mtask_head;
    cur_id = 1;
    while(obj != NULL)
    {
        if(obj->next == NULL) break;
        cur_id ++;
        obj = obj->next;
    }
    obj->next = target;
    target->id = cur_id + 1;
    target->status = MTASK_IDLE;
    target->next = NULL;
}

static mtask_t *active_task_read(void)
{
    mtask_t *obj, *active;
    active = NULL;
    obj = mtask_head;
    while(obj != NULL)
    {
        if(obj->status == MTASK_RUNNING)
        {
            active = obj;
            return obj;
        }
        else if(obj->status == MTASK_WAITING)
        {
            if(active == NULL) active = obj;
        }
        obj = obj->next;
    }
    if(active == NULL) return NULL;
    active->status = MTASK_RUNNING;
    return active;
}

static int time_ref_set(char *ptab, mtask_time_t *ref, mtask_time_t *last)
{
    int tab_len, i, value;
    char *pchar;
    pchar = ptab;
    tab_len = strlen(str);
    if(memcmp(pchar, "*", 1) == 0)
    {
        ref->sec = -1;
    }
    else if(memcmp(pchar, "*/", 2) == 0)
    {
        sscanf(pchar, "*/%d ", &value);
        ref->sec = (last->sec + value) % 60;
    }
    else if(isdigit(pchar[0]))
    {
        sscanf(pchar, "*/%d ", &value);
        if((value >= 60)||(value < 0)) return -1; 
        ref->sec = value;
    }
    
    
}

