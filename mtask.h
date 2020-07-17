#ifndef __MTASK_H__
#define __MTASK_H__


struct timer_s
{
    int cnt;
    int reload;
};

enum mtask_e
{
    MTASK_IDLE,
    MTASK_WAITING,
    MTASK_RUNNING
};

typedef struct mtask_time_s
{
    int sec;
    int min;
    int hour;
} mtask_time_t;

typedef struct mtask_s
{
    char *name;
    int id;
    enum mtask_e status;
    char *crontab;
    mtask_time_t last_time;
    int (*entry)(struct mtask_s *task_obj);
    struct mtask_s * next;
} mtask_t;

#ifdef __cplusplus
extern "C" {
#endif
void mtask_microsecond_callback(void);
int mtask_elapsed(unsigned int *ts);
int mtask_loop(void);
int mtask_active_task(const char *task_name);
#ifdef __cplusplus
}
#endif

#endif
