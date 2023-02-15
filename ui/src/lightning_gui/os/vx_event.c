
#include "vx_event.h"
#include <VX/vx_types.h>

vx_status vx_event_create(vx_event *event)
{
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    vx_event tmp_event;
    vx_status status = (vx_status)VX_SUCCESS;

    tmp_event = (vx_event)malloc(sizeof(vx_event_t));
    if(tmp_event==NULL)
    {
        *event = NULL;
        printf("Memory allocation failed\n");
        status = (vx_status)VX_ERROR_MALLOC;
    }
    else
    {
        status |= pthread_mutexattr_init(&mutex_attr);
        status |= pthread_condattr_init(&cond_attr);

        status |= pthread_mutex_init(&tmp_event->lock, &mutex_attr);
        status |= pthread_cond_init(&tmp_event->cond, &cond_attr);

        tmp_event->is_set = 0;

        if(status!=0)
        {
            pthread_cond_destroy(&tmp_event->cond);
            pthread_mutex_destroy(&tmp_event->lock);
            free(tmp_event);
            *event = NULL;
            printf("Mutex initialization failed\n");
            status = (vx_status)VX_ERROR_CREATE;
        }
        else
        {
            *event = tmp_event;
        }

        pthread_condattr_destroy(&cond_attr);
        pthread_mutexattr_destroy(&mutex_attr);
    }

    return (status);
}

vx_status vx_event_delete(vx_event *event)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;

    if(*event)
    {
        pthread_cond_destroy(&(*event)->cond);
        pthread_mutex_destroy(&(*event)->lock);
        free(*event);
        *event = NULL;
        status = (vx_status)VX_SUCCESS;
    }

    return (status);
}

vx_status vx_event_post(vx_event event)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;

    if(event)
    {
        status = pthread_mutex_lock(&event->lock);
        if(status == 0)
        {
            event->is_set = 1;

            status |= pthread_cond_signal(&event->cond);
            status |= pthread_mutex_unlock(&event->lock);
        }
        if(status != 0)
        {
            printf("Mutex post failed\n");
            status = (vx_status)VX_ERROR_DEFAULT;
        }
    }

    return (status);
}

vx_status vx_event_wait(vx_event event, uint32_t timeout)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;
    int32_t   status1;
    int32_t   retVal;

    if(event)
    {
        status = pthread_mutex_lock(&event->lock);
        if(status == 0)
        {
            vx_bool done = (vx_bool)vx_false_e;

            while(!done)
            {
                if(event->is_set==1U)
                {
                    /* clear event */
                    event->is_set = 0;
                    status = (vx_status)VX_SUCCESS;
                    done = (vx_bool)vx_true_e;
                }
                else
                if(timeout==VX_EVENT_TIMEOUT_WAIT_FOREVER)
                {
                    printf("Timeout set to TIVX_EVENT_TIMEOUT_NO_WAIT\n");
                    status = (vx_status)VX_ERROR_DEFAULT;
                    done = (vx_bool)vx_true_e;
                }
                else
                if(timeout!=VX_EVENT_TIMEOUT_WAIT_FOREVER)
                {
                    /* A valid and finite timeout has been specified. */
                    struct timespec ts;
                    struct timeval  tv;

                    retVal = gettimeofday(&tv, NULL);

                    if (retVal == 0)
                    {
                        uint32_t        sec;
                        unsigned long   micro;

                        /* timeout is expected to be in milli-sec. */
                        micro = tv.tv_usec + (timeout * 1000);
                        sec   = tv.tv_sec;

                        if (micro >= 1000000LLU)
                        {
                            micro %= 1000000LLU;
                            sec   += micro/1000000LLU;
                        }

                        ts.tv_nsec = micro * 1000;
                        ts.tv_sec  = sec;

                        retVal = pthread_cond_timedwait(&event->cond,
                                                        &event->lock,
                                                        &ts);

                        if (retVal == 110)
                        {
                            printf("Event timed-out.\n");
                            status = (vx_status)VX_ERROR_DEFAULT;
                            done = (vx_bool)vx_true_e;
                        }
                        else if (retVal)
                        {
                            /* Error other than ETIMEDOUT. */
                            printf("Event wait failed.\n");
                            status = (vx_status)VX_ERROR_DEFAULT;
                            done = (vx_bool)vx_true_e;
                        }
                    }
                    else
                    {
                        /* gettimeofday() failed. */
                        printf("gettimeofday() failed.\n");
                        status = (vx_status)VX_ERROR_DEFAULT;
                        done = (vx_bool)vx_true_e;
                    }
                }
                else
                {
                    /* timeout == TIVX_EVENT_TIMEOUT_WAIT_FOREVER */
                    retVal = pthread_cond_wait(&event->cond, &event->lock);

                    if (retVal)
                    {
                        printf("Event wait failed.\n");
                        status = (vx_status)VX_ERROR_DEFAULT;
                        done = (vx_bool)vx_true_e;
                    }
                }
            }

            status1 = pthread_mutex_unlock(&event->lock);

            if(status1 != 0)
            {
                printf("Mutex unlock failed\n");
                status = (vx_status)VX_ERROR_DEFAULT;
            }
        }
    }

    return (status);
}

vx_status vx_event_clear(vx_event event)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if(event)
    {
        status = pthread_mutex_lock(&event->lock);
        if(status == 0)
        {
            event->is_set = 0;
            status = pthread_mutex_unlock(&event->lock);
        }
        if(status != 0)
        {
            printf("Mutex lock failed\n");
            status = (vx_status)VX_ERROR_DEFAULT;
        }
    }

    return status;
}
