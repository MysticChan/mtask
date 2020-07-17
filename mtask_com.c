#include "mtask.h"
#include <time.h>

typedef struct mtask_com_s
{
    int timestamp;
    int (*read)(void *pbuf, int size);
    int (*write)(void *pbuf, int size);
    int task_use;
    int read_size;
    void *read_cache;
} mtask_com_t;

__weak void mtask_com_recv(int com_port, void *pbuf, int size)
{
    
}

