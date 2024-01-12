#ifndef _VX_TASK_
#define _VX_TASK_

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

#include "vx_event.h"

#ifdef __cplusplus
extern "C" {
#endif


#define VX_TASK_PRI_HIGHEST   (0u)
#define VX_TASK_PRI_LOWEST    (15u)
#define VX_TASK_AFFINITY_ANY  (0xFFFFu)
#define VX_MAX_TASK_NAME      (12u)
	
typedef void (*vx_task_main_f)(void *app_var);
typedef void (*vx_task_main_f_fishnet)(void *app_var , int index);

typedef struct _vx_task_create_params
{
	uint8_t *stack_ptr;

	uint32_t stack_size;

	uint32_t task_mode;

	uint32_t core_affinity;

	uint32_t priority;
	vx_task_main_f task_main;
	vx_task_main_f_fishnet task_multi;
	uint32_t fishnet_index;
	void *app_var;
	char task_name[64];
} vx_task_create_params_t;


typedef struct _vx_task_t
{
	void *tsk_handle;
	uint8_t *stack_ptr;
	uint32_t stack_size;
	uint32_t core_affinity;
	uint32_t priority;

	vx_task_main_f task_func;
	vx_task_main_f_fishnet task_multi;

	void *app_var;
	char  task_name[64];
	vx_task_create_params_t para;
} vx_task;

void vx_task_set_default_create_params(vx_task_create_params_t *params);
vx_status vx_task_create(vx_task *task, const vx_task_create_params_t *params);
vx_status vx_task_delete(vx_task *task);
void vx_task_wait_ms(uint32_t msec);

#ifdef __cplusplus
}
#endif

#endif
