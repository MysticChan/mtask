#ifndef __MTASK_COM_H__
#define __MTASK_COM_H__

typedef struct mtask_com_s
{
    char *name;
    void *cache;
    int cache_size;
    int (*write)(void *pbuf, int size);
    int (*read)(void *pbuf, int size);
    int (*close)(void);
    unsigned int timestamp;
    int recv_size;
    int tid;
    int block;
    int timeout;
    struct mtask_com_s *next;
} mtask_com_t;

#ifdef __cplusplus
extern "C" {
#endif
int mtask_com_register(mtask_com_t *com);
int mtask_com_block(const char *name);
mtask_com_t *mtask_com_open(const char *name, int task_id);

extern mtask_com_t com_rs485;
void mtask_rs485_recv_callback(void *pbuf, int size);
#ifdef __cplusplus
}
#endif

#endif
