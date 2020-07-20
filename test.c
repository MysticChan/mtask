#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int cron_range_check(unsigned char value, char *tab)
{
    int min, max;
    char *pchar = tab;
    printf("cron_range_check: %s\n", tab);
    sscanf(tab, "%d-%d", &min, &max);
    printf("min = %d max = %d\n", min, max);
    if(min < 0) return -1;
    if(max < 0) return -1;
    if((min <= value) && (max >= value)) return 0;
    return -1;
}

int cron_hash_check(unsigned char value, char *tab)
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

int cron_sec(unsigned char *value, char *tab)
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

int crontab_parse(unsigned char *value, char *tab)
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

int crontab_check(char _item[6][16]);
int main(void)
{
    char *test = "0 * * * * *";
    char item[6][16];
    char *pitem, *ptab;
    int i;
    ptab = test;
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
        // printf("item[%d] = \"%s\"\n", i, item[i]);
    }
    printf("%s\n", test);
    crontab_check(item);
    return 0;
}

typedef struct mtask_time_s
{
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char day;
    unsigned char mon;
    unsigned char week;
} mtask_time_t;

int crontab_check(char _item[6][16])
{
    int i, j;
    int cnt = 0;
    mtask_time_t ref;
    time_t rawtime, timestamp;
    struct tm *info;
    rawtime = time(NULL);
    for(i=0;i<604800;i++)
    {
        timestamp = rawtime + i;
        info = localtime(&timestamp);
        ref.sec = info->tm_sec;
        ref.min = info->tm_min;
        ref.hour = info->tm_hour;
        ref.week = info->tm_wday;
        ref.day = info->tm_mday;
        ref.mon = info->tm_mon+1;
        if(crontab_parse(&ref.sec, _item[0])) continue;
        if(crontab_parse(&ref.min, _item[1])) continue;
        if(crontab_parse(&ref.hour, _item[2])) continue;
        if(crontab_parse(&ref.day, _item[3])) continue;
        if(crontab_parse(&ref.mon, _item[4])) continue;
        if(crontab_parse(&ref.week, _item[5])) continue;
        cnt ++;
        printf("%s", asctime(info));
        if(cnt >= 7) break;
    }
    return 0;
}

#if 0
char *test_tab = "* * */2 * *";
int value;
    char *pchar;
    pchar = test_tab;
    int i;
    for(i=0;i<5;i++)
    {
        if(memcmp(pchar , "*", 1) == 0)
        {
            if(memcmp(pchar, "*/", 2) == 0)
            {
                sscanf(pchar, "*/%d ", &value);
                if((value >= 60)||(value < 0)) return -1;
                printf("%d: set */%d\n", i, value);
            }
            else 
            {
                printf("%d: set *\n", i);
            }
        }
        else if(isdigit(*pchar))
        {
            sscanf(pchar, "%d ", &value);
            if((value >= 60)||(value < 0)) return -1;
            printf("%d: set %d\n", i, value);
        }
        while(1)
        {
            if(*pchar == '\0') break;
            if(*pchar == ' ') 
            {
                while(*pchar == ' ') pchar ++;
                break;
            }
            pchar ++;
        }
    }
    if(i != 5) 
    {
        printf("i != 5\n");
        return -1;
    }
#endif
