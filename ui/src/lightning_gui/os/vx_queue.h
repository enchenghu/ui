#ifndef _VX_QUEUE_
#define _VX_QUEUE_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>


#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "vx_mutex.h"
#include "vx_event.h"


#ifdef __cplusplus
extern "C" {
#endif


#define VX_QUEUE_FLAG_BLOCK_ON_PUT        (0x00000001u)
#define VX_QUEUE_FLAG_BLOCK_ON_GET        (0x00000002u)

typedef struct _vx_queue_t 
{
  uint32_t cur_rd;
  uint32_t cur_wr;
  uint32_t count;
  uint32_t max_ele;
  uintptr_t *queue;

  vx_event block_rd;
  vx_event block_wr;
  vx_mutex lock;

  void *context;

  uint32_t flags;
  volatile vx_bool blockedOnGet;
  volatile vx_bool blockedOnPut;
} vx_queue;

vx_status vx_queue_create(
    vx_queue *queue, uint32_t max_elements, uintptr_t *queue_memory,
    uint32_t flags);
vx_status vx_queue_delete(vx_queue *queue);
vx_status vx_queue_put(vx_queue *queue, uintptr_t data, uint32_t timeout);
vx_status vx_queue_get(vx_queue *queue, uintptr_t *data, uint32_t timeout);
vx_bool vx_queue_is_empty(const vx_queue *queue);
vx_status vx_queue_peek(const vx_queue *queue, uintptr_t *data);

#ifdef __cplusplus
}
#endif

#endif
