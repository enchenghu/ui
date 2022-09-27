#ifndef _VX_MUTEX_
#define _VX_MUTEX_

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

#include "vx_event.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _vx_mutex_t 
{
	pthread_mutex_t lock;
}vx_mutex_t;

typedef struct _vx_mutex_t *vx_mutex;

vx_status vx_mutex_create(vx_mutex *mutex, int mute_type);
vx_status vx_mutex_delete(vx_mutex *mutex);
vx_status vx_mutex_lock(vx_mutex mutex);
vx_status vx_mutex_unlock(vx_mutex mutex);

#ifdef __cplusplus
}
#endif

#endif
