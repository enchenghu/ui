#ifndef _VX_EVENT_H_
#define _VX_EVENT_H_

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

#include <VX/vx.h>
#include <VX/vx_api_1.h>
#include <VX/vxu.h>
#include <VX/vx_ext_cadence.h>
#include <VX/vx_types.h>


#ifdef __cplusplus
extern "C" {
#endif


#define VX_EVENT_TIMEOUT_WAIT_FOREVER     (0xFFFFFFFFu)
#define VX_EVENT_TIMEOUT_NO_WAIT          (0u)


typedef struct _vx_event_t 
{
	uint16_t is_set;
	pthread_mutex_t lock;
	pthread_cond_t	cond;

} vx_event_t;

typedef enum
{
	VX_ERROR_CREATE = -1,
	VX_ERROR_MALLOC = -2,
	VX_ERROR_CREATE_KEY = -3,
	VX_ERROR_CREATE_GET = -4,
	VX_ERROR_CREATE_CTL = -5,
	VX_ERROR_DEFAULT = -6,
	VX_ERROR_DELETE = -7,
}vx_status_t;

typedef struct _vx_event_t* vx_event;

vx_status vx_event_create(vx_event *event);
vx_status vx_event_delete(vx_event *event);
vx_status vx_event_post(vx_event event);
vx_status vx_event_wait(vx_event event, uint32_t timeout);
vx_status vx_event_clear(vx_event event);


#ifdef __cplusplus
}
#endif

#endif
