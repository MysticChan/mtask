#include "mtask.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

mtask_t *mtask_head = NULL;
static int active_task_list(void);
static int active_task_run(void);

int mtask_loop(void)
{
    active_task_list();
    active_task_run();
    return 0;
}

int mtask_active_task(int tid)
{
    mtask_t *obj;
    obj = mtask_head;
    while(obj != NULL)
    {
        if(obj->tid == tid)
        {
            if(obj->status != MTASK_SLEEP) break;
            obj->status = MTASK_RUNNING;
            obj->step = 0;
            mtask_localtime_read(&obj->exec_time);
            return 0;
        }
        obj = obj->next;
    }
    return -1;
}

int mtask_register(mtask_t *target, const char *crontab, entry_fun_t entry)
{
    mtask_t *obj;
    int cur_id = 1;
    if(mtask_head == NULL)
    {
        mtask_head = target;
        target->tid = cur_id;
    }
    else
    {
        obj = mtask_head;
        while(obj != NULL)
        {
            if(obj->next == NULL) break;
            cur_id ++;
            obj = obj->next;
        }
        obj->next = target;
        target->tid = cur_id + 1;
    }

    target->status = MTASK_SLEEP;
    target->step = 0;
    target->crontab = (char *)crontab;
    target->entry = entry;
    memset(&target->exec_time, 0, sizeof(mtask_time_t));
    target->next = NULL;

    return target->tid;
}

static int cron_range_check(unsigned char value, char *tab)
{
    int min, max;
    sscanf(tab, "%d-%d", &min, &max);
    if(min < 0) return -1;
    if(max < 0) return -1;
    if((min <= value) && (max >= value)) return 0;
    return -1;
}

static int cron_hash_check(unsigned char value, char *tab)
{
    char *pchar;
    int temp;
    pchar = tab;
    while(*pchar != '\0')
    {
        if(isdigit(*pchar))
        {
            sscanf(pchar, "%d", &temp);
            if(value == temp) return 0;
            while(isdigit(*pchar)) pchar ++;
        }
        pchar ++;
    }
    return -1;
}

static int crontab_parse(unsigned char *value, char *tab)
{
    int temp;
    char *pchar;
    pchar = tab;
    if(memcmp(pchar, "*", 1) == 0)
    {
        if(memcmp(pchar, "*/", 2) == 0)
        {
            sscanf(pchar, "*/%d", &temp);
            if(*value % temp != 0) return -1;
        }
        return 0;
    }
    if(isdigit(*pchar))
    {
        while((*pchar != '-')&&(*pchar != ',')&&(*pchar != '\0')) pchar ++;
        if(*pchar == '-')
        {
            return cron_range_check(*value, tab);
        }
        else if(*pchar == ',')
        {
            return cron_hash_check(*value, tab);
        }
        else
        {
            sscanf(tab, "%d", &temp);
            if(*value == temp) return 0;
        }
    }
    return -1;
}

static int active_task(mtask_time_t *cur_time, char *crontab)
{
    int i;
    char item[6][16];
    char *pitem, *ptab;
    unsigned char *pref;
    ptab = crontab;
    memset(item, 0, sizeof(item));
    for(i=0;i<6;i++)
    {
        pitem = item[i];
        while((*ptab != ' ') && (*ptab != '\0'))
        {
            *pitem = *ptab;
            pitem ++;
            ptab ++;
        }
        while(*ptab == ' ') ptab ++;
        if(strlen(item[i]) == 0) return -1;
    }
    pref = (unsigned char *)cur_time;
    for(i=0;i<6;i++)
        if(crontab_parse(&pref[i], item[i])) return -1;
    return 0;
}

static int active_task_list(void)
{
    mtask_t *obj;
    static mtask_time_t _rec_time;
    mtask_time_t cur_time;
    mtask_localtime_read(&cur_time);
    if(memcmp(&_rec_time, &cur_time, sizeof(mtask_time_t)) == 0) return -1;
    memcpy(&_rec_time, &cur_time, sizeof(mtask_time_t));
    obj = mtask_head;
    while(obj != NULL)
    {
        if((obj->status == MTASK_SLEEP) && (obj->crontab != NULL))
        {
            if(active_task(&cur_time, obj->crontab) == 0)
            {
                memcpy(&obj->exec_time, &cur_time, sizeof(mtask_time_t));
                obj->status = MTASK_RUNNING;
                obj->step = 0;
            }
        }
        obj = obj->next;
    }
    return 0;
}

static int active_task_run(void)
{
    mtask_t *obj;
    obj = mtask_head;
    while(obj != NULL)
    {
        if(obj->status == MTASK_RUNNING)
        {
            if(obj->entry(obj) == 0)
                obj->status = MTASK_SLEEP;
        }
        obj = obj->next;
    }
    return 0;
}

