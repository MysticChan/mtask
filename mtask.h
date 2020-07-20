#ifndef __MTASK_H__
#define __MTASK_H__
enum mtask_e
{
    MTASK_SLEEP,
    MTASK_WAITING,
    MTASK_RUNNING
};

typedef struct mtask_time_s
{
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char week;
    unsigned char day;
    unsigned char mon;
} mtask_time_t;

typedef struct mtask_s
{
    int tid;
    char *name;
    enum mtask_e status;
    char *crontab;
    mtask_time_t last_exec;
    int (*entry)(struct mtask_s *task_obj);
    struct mtask_s * next;
} mtask_t;

typedef int (*entry_fun_t)(mtask_t *obj);

#ifdef __cplusplus
extern "C" {
#endif

int mtask_loop(void);
int mtask_active_task(const char *task_name);

int mtask_localtime(mtask_time_t *_time);
unsigned int mtask_tick(void);
unsigned int mtask_tick_elapsed(unsigned int *ts);

#ifdef __cplusplus
}
#endif

#endif
