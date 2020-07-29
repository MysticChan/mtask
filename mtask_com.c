#include "mtask.h"
#include "mtask_com.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_TIMEOUT 2000
mtask_com_t *com_head = NULL;

int mtask_com_register(mtask_com_t *com)
{
    mtask_com_t *obj;
    if(com_head == NULL)
    {
        com_head = com;
    }
    else
    {
        obj = com_head;
        while(obj != NULL)
        {
            if(obj->next == NULL) break;
            obj = obj->next;
        }
    }
    com->timestamp = 0;
    com->recv_size = 0;
    com->tid = 0;
    com->timeout = DEFAULT_TIMEOUT;
    com->next = NULL;
    return 0;
}

int mtask_com_block(const char *name)
{
    mtask_com_t *obj;
    obj = com_head;
    while(obj != NULL)
    {
        if(strcmp(obj->name, name) == 0)
        {
            if(!obj->block)
            {
                DEBUG("block %s.\n", obj->name);
            }
            obj->timestamp = mtask_ticks_read();
            obj->block = 1;
            return 0;
        }
        obj = obj->next;
    }
    return -1;
}

mtask_com_t *mtask_com_open(const char *name, int task_id)
{
    mtask_com_t *obj;
    obj = com_head;
    while(obj != NULL)
    {
        if(strcmp(obj->name, name) == 0)
        {
            if(obj->block)
            {
                if(mtask_ticks_elapsed(&obj->timestamp) > 15000)
                {
                    obj->block = 0;
                    DEBUG("release %s.\n", obj->name);
                }
                return NULL;
            }
            if(!obj->tid)
            {
                obj->tid = task_id;
                return obj;
            }
            else
            {
                if(obj->tid == task_id) return obj;
                return NULL;
            }
        }
        obj = obj->next;
    }
    return NULL;
}
#if 0
static int _rs485_write(void *pbuf, int size)
{
    DEBUG("tid(%d) rs485 send (%d): ", com_rs485.tid, size);
    DEBUG_BUF(pbuf, size);
    com_rs485.timestamp = mtask_ticks_read();
    com_rs485.recv_size = 0;
    return rs485_write(pbuf, size);
}

static int _rs485_read(void *pbuf, int size)
{
    int cpy_size;
    if(mtask_ticks_elapsed(&com_rs485.timestamp) > com_rs485.timeout) return -1;
    if(pbuf == NULL) return 0;
    if(com_rs485.recv_size == 0) return 0;
    if(com_rs485.recv_size > size) cpy_size = size;
    else cpy_size = com_rs485.recv_size;
    com_rs485.timestamp = mtask_ticks_read();
    memcpy(pbuf, com_rs485.cache, cpy_size);
    com_rs485.recv_size = 0;
    DEBUG("tid(%d) rs485 recv (%d): ", com_rs485.tid, cpy_size);
    DEBUG_BUF(pbuf, cpy_size);
    return cpy_size;
}

static int _rs485_close(void)
{
    if(com_rs485.tid) com_rs485.tid = 0;
    return 0;
}

unsigned char rs485_cache[512];
mtask_com_t com_rs485 =
{
    .name = "RS485",
    .cache = rs485_cache,
    .cache_size = 512,
    .write = _rs485_write,
    .read = _rs485_read,
    .close = _rs485_close,
};

void mtask_rs485_recv_callback(void *pbuf, int size)
{
    int cpy_size;
    if(!size) return;
    if(com_rs485.block) return;
    RS485_20s_Flag = 0;
    if(size > com_rs485.cache_size) cpy_size = com_rs485.cache_size;
    else cpy_size = size;
    memcpy(com_rs485.cache, pbuf, cpy_size);
    com_rs485.recv_size = cpy_size;
}
#endif
