
#include "vx_queue.h"

typedef struct _vx_queue_context *vx_queue_context;

typedef struct _vx_queue_context 
{
  pthread_mutex_t lock;
  pthread_cond_t  condGet;
  pthread_cond_t  condPut;
} vx_queue_context_t;



vx_status vx_queue_create(
    vx_queue *queue, uint32_t max_elements, uintptr_t *queue_memory,
    uint32_t flags)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;
    vx_queue_context context = NULL;

    if ((NULL != queue) && (NULL != queue_memory) && (0U != max_elements))
    {
        /*
         * init queue to 0's
         */
        memset(queue, 0, sizeof(vx_queue));

        /*
         * init queue with user parameters
         */
        queue->max_ele = max_elements;
        queue->flags = flags;

        queue->queue = queue_memory;

        queue->context = malloc(sizeof(vx_queue_context_t));

        context = queue->context;

        if(queue->context==NULL)
        {
            //printf("queue memory allocation failed\n");
            status = (vx_status)VX_ERROR_MALLOC;
        }
        else
        {
            pthread_mutexattr_t mutex_attr;

            status = (vx_status)VX_SUCCESS;

            status |= pthread_mutexattr_init(&mutex_attr);
            status |= pthread_mutex_init(&context->lock, &mutex_attr);

            pthread_mutexattr_destroy(&mutex_attr);

            if(status==0)
            {
                if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_GET)
                {
                    pthread_condattr_t cond_attr;

                    /*
                     * user requested block on que get
                     */

                    /*
                     * create cond for it
                     */
                    status |= pthread_condattr_init(&cond_attr);
                    status |= pthread_cond_init(&context->condGet, &cond_attr);

                    pthread_condattr_destroy(&cond_attr);
                }
                if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_PUT)
                {
                    pthread_condattr_t cond_attr;

                    /*
                     * user requested block on que put
                     */

                    /*
                     * create cond for it
                     */
                    status |= pthread_condattr_init(&cond_attr);
                    status |= pthread_cond_init(&context->condPut, &cond_attr);

                    pthread_condattr_destroy(&cond_attr);
                }
            }
            if ((vx_status)(vx_status)VX_SUCCESS == status)
            {
                queue->blockedOnGet = (vx_bool)vx_false_e;
                queue->blockedOnPut = (vx_bool)vx_false_e;
            }
            else
            {
                pthread_mutex_destroy(&context->lock);
                free(queue->context);
                queue->context = NULL;
            }
        }
    }

    return (status);
}

vx_status vx_queue_delete(vx_queue *queue)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;
    vx_queue_context context;

    if (NULL != queue && queue->context != NULL)
    {
        context = queue->context;

        //printf("if this hangs, please ensure all application threads have been destroyed\n");
        if ((queue->flags & VX_QUEUE_FLAG_BLOCK_ON_GET))
        {
            pthread_cond_destroy(&(context)->condGet);
        }
        if ((queue->flags & VX_QUEUE_FLAG_BLOCK_ON_PUT))
        {
            pthread_cond_destroy(&(context)->condPut);
        }
        pthread_mutex_destroy(&context->lock);

        free(context);
        queue->context = NULL;

        status = (vx_status)VX_SUCCESS;
    }

    return (status);
}

vx_status vx_queue_put(vx_queue *queue, uintptr_t data, uint32_t timeout)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;
    volatile vx_bool do_break = (vx_bool)vx_false_e;
    vx_queue_context context = NULL;

    if(queue && queue->context)
    {
        context = queue->context;

        status = pthread_mutex_lock(&context->lock);
        if(status==0)
        {
            do
            {
                if (queue->count < queue->max_ele)
                {
                    /* insert element */
                    queue->queue[queue->cur_wr] = data;

                    /* increment put pointer */
                    queue->cur_wr = (queue->cur_wr + 1U) % queue->max_ele;

                    /* increment count of number element in que */
                    queue->count++;

                    /* mark status as success */
                    status = (vx_status)VX_SUCCESS;

                    if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_GET)
                    {
                        /* blocking on que get enabled */

                        /* post cond to unblock, blocked tasks */
                        pthread_cond_signal(&context->condGet);
                    }

                    /* exit, with success */
                    do_break = (vx_bool)vx_true_e;
                }
                else
                {
                    /* que is full */
                    if (timeout == VX_EVENT_TIMEOUT_NO_WAIT)
                    {
                        //printf("queue is full\n");
                        status = (vx_status)VX_ERROR_DEFAULT;
                        do_break = (vx_bool)vx_true_e; /* non-blocking, so exit with error */
                    }
                    else if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_GET)
                    {
                        /* blocking on que put enabled */
                        queue->blockedOnPut = (vx_bool)vx_true_e;
                        pthread_cond_wait(&context->condPut, &context->lock);
                        queue->blockedOnPut = (vx_bool)vx_false_e;
                    }
                    else
                    {
                        /* blocking on que put disabled */

                        /* exit with error */
                        //printf("blocking on queue is disabled\n");
                        status = (vx_status)VX_ERROR_DEFAULT;
                        do_break = (vx_bool)vx_true_e;
                    }
                }

                if ((vx_bool)(vx_bool)vx_true_e == do_break)
                {
                    break;
                }
            }
            while (1);

            status |= pthread_mutex_unlock(&context->lock);
        }
    }

    return (status);
}

vx_status vx_queue_get(vx_queue *queue, uintptr_t *data, uint32_t timeout)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;/* init status to error */
    volatile vx_bool do_break = (vx_bool)vx_false_e;
    vx_queue_context context = NULL;

    if(queue && queue->context)
    {
        context = queue->context;

        status = pthread_mutex_lock(&context->lock);
        if(status==0)
        {
            do
            {
                if (queue->count > 0U)
                {
                    /* extract the element */
                    *data = queue->queue[queue->cur_rd];

                    /* increment get pointer */
                    queue->cur_rd = (queue->cur_rd + 1U) % queue->max_ele;

                    /* decrmeent number of elements in que */
                    queue->count--;

                    /* set status as success */
                    status = (vx_status)VX_SUCCESS;

                    if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_PUT)
                    {
                        /* post cond to unblock, blocked tasks */
                        pthread_cond_signal(&context->condPut);
                    }

                    /* exit with success */
                    do_break = (vx_bool)vx_true_e;
                }
                else
                {
                    /* no elements or not enough element in que to extract */
                    if (timeout == VX_EVENT_TIMEOUT_NO_WAIT)
                    {
                        status = (vx_status)VX_ERROR_DEFAULT;
                        do_break = (vx_bool)vx_true_e; /* non-blocking, exit with error */
                    }
                    else
                    if (queue->flags & VX_QUEUE_FLAG_BLOCK_ON_GET)
                    {
                        /* blocking on que get enabled */

                        queue->blockedOnGet = (vx_bool)vx_true_e;
                        pthread_cond_wait(&context->condGet, &context->lock);
                        queue->blockedOnGet = (vx_bool)vx_false_e;
                        /* received semaphore, check que again */
                    }
                    else
                    {
                        /* blocking on que get disabled */

                        /* exit with error */
                        //printf("blocking on que get disabled\n");
                        status = (vx_status)VX_ERROR_DEFAULT;
                        do_break = (vx_bool)vx_true_e;
                    }
                }

                if ((vx_bool)(vx_bool)vx_true_e == do_break)
                {
                    break;
                }
            }
            while (1);
            status |= pthread_mutex_unlock(&context->lock);
        }
    }
    return (status);
}

vx_bool vx_queue_is_empty(const vx_queue *queue)
{
    vx_bool is_empty = (vx_bool)vx_true_e;
    vx_queue_context context = NULL;
    vx_status status;

    if(queue && queue->context)
    {
        context = queue->context;

        status = pthread_mutex_lock(&context->lock);
        if(status==0)
        {
            if (queue->count == 0U)
            {
                is_empty = (vx_bool)vx_true_e;
            }
            else
            {
                is_empty = (vx_bool)vx_false_e;
            }
            pthread_mutex_unlock(&context->lock);
        }
    }
    return (is_empty);
}

vx_status vx_queue_peek(const vx_queue *queue, uintptr_t *data)
{
    vx_status status = (vx_status)VX_ERROR_DEFAULT;/* init status to error */
    vx_queue_context context = NULL;

    if(queue && queue->context)
    {
        context = queue->context;

        status = pthread_mutex_lock(&context->lock);
        if(status==0)
        {
            if (queue->count > 0U)
            {
                /* 'peek' the element but dont extract it */
                *data = queue->queue[queue->cur_rd];

                /* set status as success */
                status = (vx_status)VX_SUCCESS;
            }

            status = pthread_mutex_unlock(&context->lock);
        }
    }
    return (status);
}
