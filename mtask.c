#include "mtask.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

mtask_t *mtask_head = NULL;
static mtask_t *active_task_read(void);
int mtask_init(void)
{
    // time init
    // mtask_rawtime
}

int mtask_loop(void)
{
    mtask_t *mtask_run;
    mtask_run = active_task_read();
    if(mtask_run != NULL)
    {
        if(mtask_run->entry(mtask_run) == 0)
            mtask_run->status = MTASK_SLEEP;
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
            if(obj->status != MTASK_SLEEP) break;
            obj->status = MTASK_WAITING;
            return 0;
        }
        obj = obj->next;
    }
    return -1;
}

int mtask_register(mtask_t *target, const char *name, const char *crontab, int (*entry)(mtask_t *obj))
{
    mtask_t *obj;
    int cur_id = 1;
    if(mtask_head == NULL) 
    {
        mtask_head = target;
        target->id = cur_id;   
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
        target->id = cur_id + 1;
    }

    target->status = MTASK_SLEEP;
    target->crontab = (char *)crontab;
    target->entry = entry;
    memset(&target->last_exec, 0, sizeof(mtask_time_t));
    target->next = NULL;
    
    return 0;
}

static int cron_hash_check(unsigned char value, char *tab)
{
    char *pchar;
    int temp;
    printf("cron_hash_check: %s\n", tab);
    pchar = tab;
    while(*pchar != '\0')
    {
        if(isdigit(*pchar))
        {
            sscanf(pchar, "%d", &temp);
            printf("cron_hash_check: temp = %d\n", temp);
            if(value == temp) return 0;
            while(isdigit(*pchar)) pchar ++;
        }
        printf("cron_hash_check: %c\n", *pchar);
        pchar ++;
    }
    return -1;
}

static int cron_sec(unsigned char *value, char *tab)
{
    int temp;
    char *pchar;
    pchar = tab;
    if(memcmp(pchar, "*", 1) == 0)
    {
        if(memcmp(pchar, "*/", 2) == 0)
        {
            sscanf(pchar, "*/%d", &temp);
            if((temp > 60) || (temp < 0)) return -1;
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
            printf("temp = %d\n", temp);
            if((temp > 60) || (temp < 0)) return -1;
            if(*value == temp) return 0;
        }
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
    obj = mtask_head;
    while(obj != NULL)
    {
        if((obj->status == MTASK_SLEEP) && (obj->crontab != NULL))
        {
            if(active_task(&cur_time, obj->crontab) == 0)
            {
                memcpy(obj->last_exec, &cur_time, sizeof(mtask_time_t));
                obj->status = MTASK_WAITING;
            }
        }
        obj = obj->next;
    }
    return 0;
}

static mtask_t *active_task_read(void)
{
    mtask_t *obj, *active;

    active_task_list();

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

