
#include "vx_mutex.h"

vx_status vx_mutex_create(vx_mutex *mutex, int mute_type)
{
    vx_status status = (vx_status)VX_SUCCESS;
	pthread_mutexattr_t mutex_attr;
	vx_mutex tmp_mutex;

    tmp_mutex = (vx_mutex)malloc(sizeof(vx_mutex_t));
    if(tmp_mutex==NULL)
    {
        *mutex = NULL;
        printf("Mutex memory allocation failed\n");
        status = (vx_status)VX_ERROR_MALLOC;
    }
    else
    {
        status |= pthread_mutexattr_init(&mutex_attr);
        status |= pthread_mutexattr_setpshared(&mutex_attr, mute_type);
        status |= pthread_mutex_init(&tmp_mutex->lock, &mutex_attr);

        if(status != 0)
        {
            pthread_mutex_destroy(&tmp_mutex->lock);
            free(tmp_mutex);
            *mutex = NULL;
            printf("Mutex initialization failed\n");
            status = (vx_status)VX_ERROR_CREATE;
        }
        else
        {
            *mutex = tmp_mutex;
        }
        pthread_mutexattr_destroy(&mutex_attr);
    }

    return (status);
}

vx_status vx_mutex_delete(vx_mutex *mutex)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if(*mutex)
    {
        pthread_mutex_destroy(&(*mutex)->lock);
        free(*mutex);
        *mutex = NULL;
        status = (vx_status)VX_SUCCESS;
    }

    return (status);
}

vx_status vx_mutex_lock(vx_mutex mutex)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if(mutex)
    {
        status = pthread_mutex_lock(&mutex->lock);
        if(status != 0)
        {
            printf("Mutex lock failed\n");
            status = (vx_status)VX_ERROR_DEFAULT;
        }
    }

    return (status);
}

vx_status vx_mutex_unlock(vx_mutex mutex)
{
    vx_status status = (vx_status)VX_SUCCESS;

    if(mutex)
    {
        status = pthread_mutex_unlock(&mutex->lock);
        if(status != 0)
        {
            printf("Mutex unlock failed\n");
            status = (vx_status)VX_ERROR_DEFAULT;
        }
    }

    return (status);
}



