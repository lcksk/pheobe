
/* ---------------------------------------------
 * author: malka, buttonfly@gmail.com *
 * release date: 2008.4.14
 * --------------------------------------------*/

#include "linkedlist.h"
#include "workerthread.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    void (*run)(void*);
    void* arg;
    void (*complete_callback)(void*);
} unittask_t;

#define WORKERTHREAD_CLOSED            1

typedef struct
{
    linkedlist            linkedlist_thread;
    linkedlist            linkedlist_workqueue;
    int                    closed;
    pthread_attr_t        attribute;
    pthread_mutex_t        mutex;
    pthread_cond_t        cond;
} workerthread_t;

/**
 * create the unit task with the specified routine, arg, and complete callback
 * you must not call the function unittask_close after use because this workerthread will do it.
 * param routine
 * param arg
 * param complete_callback
 * return the unittask which is composed with the specified arguments
 */
static unittask_t* unittask_new(void (*routine)(void*), void* arg, void(*complete_callback)(void*));

/**
 * close the unittask structure
 * param unittask the unittask to be closed
 */
static void unittask_delete(unittask_t*);


/**
 * You ain't likely to call this function at all, I guess.
 * param workerthread this worker thread pool
 * return unittask you queued.
 */
static unittask_t* workerthread_get_task(workerthread workerthread);


//static void* work(void* _workerthread)
void* work(workerthread _workerthread)
{
    workerthread_t* workerthread;
    unittask_t* task;
    workerthread = (workerthread_t*) _workerthread;

    while(workerthread->closed != WORKERTHREAD_CLOSED)
    {

        task = (unittask_t*) workerthread_get_task(workerthread);
        if(task == NULL)
        {
        }
        else
        {
            task->run(task->arg);
            if(task->complete_callback != NULL)
            {
                task->complete_callback(task->arg);
            }
            unittask_delete(task);
        }
    }
    pthread_exit((void*) NULL);
    return NULL;
}

workerthread workerthread_new(int numofthread)
{
    workerthread_t*     workerthread;

    int                 size;
    int                 i;
    pthread_t            thread;

    size = sizeof(workerthread_t);
    workerthread = (workerthread_t*) malloc(size);
    memset(workerthread, 0, size);
    workerthread->linkedlist_thread = linkedlist_new();
    workerthread->linkedlist_workqueue = linkedlist_new();
    workerthread->closed = 0;

    pthread_attr_init(&workerthread->attribute);
    pthread_mutex_init(&workerthread->mutex, NULL/*&workerthread->attribute*/);
    pthread_cond_init(&workerthread->cond, NULL);

    pthread_attr_setscope(&workerthread->attribute, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&workerthread->attribute, PTHREAD_CREATE_JOINABLE);
    size_t stacksize;
    pthread_attr_getstacksize(&workerthread->attribute, &stacksize);
//    M_("[workerthread][create] stack-size: %d", stacksize);
    pthread_attr_setstacksize(&workerthread->attribute, stacksize * 4);

//    pthread_mutex_lock(&workerthread->mutex);
    for(i = 0; i < numofthread; i++)
    {
        pthread_create(&thread, &workerthread->attribute, work, (void*)workerthread);
        linkedlist_add(workerthread->linkedlist_thread, (void*)thread);
    }
//    pthread_mutex_unlock(&workerthread->mutex);
    return workerthread;
}

void workerthread_delete(workerthread _workerthread)
{
    workerthread_t*     workerthread;

    workerthread = (workerthread_t*) _workerthread;


    // in order to kill threads.
    pthread_mutex_lock(&(workerthread->mutex));
    workerthread->closed = WORKERTHREAD_CLOSED;
    pthread_cond_broadcast(&(workerthread->cond));
    pthread_mutex_unlock(&(workerthread->mutex));

    pthread_t    thread;
    void* data;
    for(;(data = linkedlist_remove_at(workerthread->linkedlist_thread, 0)) != NULL;)
    {
        thread = (pthread_t) data;
        pthread_join(thread, NULL);
    }

    //    pthread_mutex_lock(&que->mutex);

    if(linkedlist_size(workerthread->linkedlist_workqueue) > 0)
    {
        for(; (data = linkedlist_remove_at(workerthread->linkedlist_workqueue, 0)) != NULL;)
        {
            if(data != NULL)
            {
                free(data);
            }
        }
    }

    pthread_attr_destroy(&(workerthread->attribute));
    pthread_mutex_destroy(&(workerthread->mutex));
    pthread_cond_destroy(&(workerthread->cond));
    linkedlist_delete(workerthread->linkedlist_workqueue, NULL);

    free(workerthread);
    return;
}

unittask_t* unittask_new(void (*routine)(void*), void* arg, void(*complete_callback)(void*))
{
    unittask_t* task;
    int size;

    size = sizeof(unittask_t);
    task = (unittask_t*) malloc(size);
    memset(task, 0, size);
    task->run = routine;
    task->arg = arg;
    task->complete_callback = complete_callback;
    return task;
}

void unittask_delete(unittask_t* task)
{
    free(task);
    return;
}

//void workerthread_put_task(void* _workerthread, unittask_t* task)
void workerthread_put_task(workerthread _workerthread, void (*routine)(void*), void* arg, void(*complete_callback)(void*))
{
    workerthread_t*    workerthread;
    workerthread = (workerthread_t*) _workerthread;

    pthread_mutex_lock(&workerthread->mutex);
    linkedlist_add(workerthread->linkedlist_workqueue, unittask_new(routine, arg, complete_callback));
//    pthread_cond_broadcast(&workerthread->cond);
    pthread_cond_signal(&workerthread->cond);
    pthread_mutex_unlock(&workerthread->mutex);
    return;
}

unittask_t* workerthread_get_task(workerthread _workerthread)
{
    workerthread_t*    workerthread;
    workerthread = (workerthread_t*) _workerthread;
    unittask_t*    task;

    pthread_mutex_lock(&workerthread->mutex);
    for(;;)
    {
        int size = linkedlist_size(workerthread->linkedlist_workqueue);
        if(size > 0)
        {
            break;
        }

        pthread_cond_wait(&workerthread->cond, &workerthread->mutex);

        if(workerthread->closed == WORKERTHREAD_CLOSED)
        {
            pthread_mutex_unlock(&workerthread->mutex);
            pthread_exit((void*) NULL);
        }
    }

    task = (unittask_t*)linkedlist_remove_at(workerthread->linkedlist_workqueue, 0);
    pthread_mutex_unlock(&workerthread->mutex);
    return task;
}
