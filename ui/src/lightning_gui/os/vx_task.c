
#include "vx_task.h"

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   /* for nanosleep */

int nanosleep(const struct timespec *req, struct timespec *rem);

#else
#include <unistd.h> /* for usleep */

extern int usleep (__useconds_t __useconds);

#endif

#define PRI_MAX  sched_get_priority_max(SCHED_FIFO)
#define PRI_MIN  sched_get_priority_min(SCHED_FIFO)

typedef struct _vx_task_context
{
	pthread_t hndl;

} vx_task_context_t;

typedef struct _vx_task_context *vx_task_context;


void vx_task_set_default_create_params(vx_task_create_params_t *params)
{
    if (NULL != params)
    {
        memset(params, 0, sizeof(vx_task_create_params_t));

        params->core_affinity = VX_TASK_AFFINITY_ANY;
        params->priority = VX_TASK_PRI_LOWEST;
    }
}

static void *vx_task_main(void *arg)
{
    vx_task *task = (vx_task*)arg;

    //printf("task->para.task_mode is %d\n", task->para.task_mode);

    if( task && (task->task_func || task->task_func_fishnet))
    {
        switch(task->para.task_mode){
            case 0:
                task->task_func(task->app_var);
                break;
            case 1:
                task->task_func_fishnet(task->app_var, task->para.fishnet_index);
                break;
            default:
                break;
        }
    }

    return NULL;
}

vx_status vx_task_create(vx_task *task, const vx_task_create_params_t *params)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if ((NULL != task) && (NULL != params))
    {
        vx_task_context context;

        task->tsk_handle = NULL;

        context = malloc(sizeof(vx_task_context_t));
        if(context == NULL)
        {
            //printf("Context memory allocation failed\n");
            status = (vx_status)VX_ERROR_MALLOC;
        }
        else
        {
            pthread_attr_t thread_attr;

            task->stack_ptr = params->stack_ptr;
            task->stack_size = params->stack_size;
            task->core_affinity = params->core_affinity;
            task->priority = params->priority;
            task->task_func = params->task_main;
            task->app_var = params->app_var;
            task->para.task_mode = params->task_mode;
            if (params->task_mode == 1){
                task->task_func_fishnet = params->task_main_fishnet;
                task->para.fishnet_index = params->fishnet_index;
            }

            status = pthread_attr_init(&thread_attr);

            if(status==0)
            {
                #if 0
                {
                    struct sched_param schedprm;
                    uint32_t pri;

                    if(task->stack_size>0)
                    {
                        status |= pthread_attr_setstacksize(&thread_attr, task->stack_size);
                    }
                    status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);

                    pri = PRI_MIN;
                    if(task->priority==TIVX_TASK_PRI_HIGHEST)
                    {
                        pri = PRI_MAX;
                    }
                    else
                    if(task->priority==TIVX_TASK_PRI_LOWEST)
                    {
                        pri = PRI_MIN;
                    }

                    schedprm.sched_priority = pri;
                    status |= pthread_attr_setschedparam(&thread_attr, &schedprm);
                }
                #endif

                if(status==0)
                {
                    //printf("will enter pthread_create\n");
                    status = pthread_create(&context->hndl, &thread_attr, vx_task_main, task);
                    //printf(" finished pthread_create, status is %d\n", status);

                }
                pthread_attr_destroy(&thread_attr);
            }
            if (status == 0)
            {
                task->tsk_handle = (void *)context;
            }
            else
            {
                printf("!!!!!!error!!!!pthread_create failed\n");
                free(context);
            }
        }
    }
    else
    {
        //printf("Task or params arguments are NULL\n");
        status = (vx_status)VX_ERROR_DEFAULT;
    }

    return (status);
}

vx_status vx_task_delete(vx_task *task)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if(task && task->tsk_handle)
    {
        vx_task_context context;
        void *ret_val;

        context = (vx_task_context)task->tsk_handle;

        pthread_cancel(context->hndl);
        pthread_join(context->hndl, &ret_val);

        free(context);
        task->tsk_handle = NULL;
    }

    return (status);
}

void vx_task_wait_ms(uint32_t msec)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec delay_time, remain_time;
    int ret;

    delay_time.tv_sec  = msec/1000U;
    delay_time.tv_nsec = (msec%1000U)*1000000U;

    do
    {
        ret = nanosleep(&delay_time, &remain_time);
        if(ret < 0 && remain_time.tv_sec > 0 && remain_time.tv_nsec > 0)
        {
            /* restart for remaining time */
            delay_time = remain_time;
        }
        else
        {
            break;
        }
    } while(1);
#else
    usleep(msec * 1000);
#endif
}

