/*
 * lazyworker.c
 *
 *  Created on: Jun 2, 2011
 *      Author: buttonfly
 */

#include "lazyworker.h"
#include <pthread.h>
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum
{
    eThreadStatus_RUNNING,
    eThreadStatus_CLOSED,
} eThreadStatus;

typedef struct
{
    void*                                 ptr;
    lazyworker_workCallback fp;
    linkedlist                            sth_todo;
    pthread_cond_t                cond;
    pthread_mutex_t             mutex;
    pthread_t                        thread;
    eThreadStatus            threadStatus;
    struct timespec            time_wait;
    void*                                args;
} _lazyworker_t;

typedef struct
{
    struct timeval t;
} _unit;


static void work(_lazyworker_t* worker);

lazyworker lazyworker_new(struct timespec* time_wait, lazyworker_workCallback fp, void* args)
{
	_lazyworker_t* worker = (_lazyworker_t*) malloc(sizeof(_lazyworker_t));
	memset(worker, 0, sizeof(_lazyworker_t));
	worker->sth_todo = linkedlist_new();
	pthread_mutex_init(&worker->mutex, NULL);
	pthread_cond_init(&worker->cond, NULL);
	worker->threadStatus = eThreadStatus_RUNNING;
	worker->time_wait.tv_sec = time_wait->tv_sec;
	worker->time_wait.tv_nsec = time_wait->tv_nsec;
	worker->fp = fp;
	worker->args = args;
	pthread_create(&worker->thread, NULL, (void*(*)(void*))work, worker);
	return (lazyworker) worker;
}

void lazyworker_invokeLater(lazyworker lazy)
{
    if(lazy == NULL)
    {
        fprintf(stderr, "NULL\n");
        return;
    }
    _lazyworker_t* worker = (_lazyworker_t*) lazy;
    pthread_mutex_lock(&worker->mutex);

    _unit* u = (_unit*) malloc(sizeof(_unit));
    gettimeofday(&u->t, NULL);
    linkedlist_add(worker->sth_todo, u);
    int size;
    size = linkedlist_size(worker->sth_todo);
    if(size > 1)
    {
        _unit* unit = (_unit*)linkedlist_remove_at(worker->sth_todo, 0);
        free(unit);
    }
    pthread_cond_broadcast(&worker->cond);
    pthread_mutex_unlock(&worker->mutex);
    return;
}

void lazyworker_delete(lazyworker lazy)
{
    if(lazy == NULL)
    {
        fprintf(stderr, "NULL\n");
    }
    _lazyworker_t* worker = (_lazyworker_t*) lazy;
    pthread_mutex_lock(&worker->mutex);
    worker->threadStatus = eThreadStatus_CLOSED;
    pthread_cond_broadcast(&worker->cond);
    pthread_mutex_unlock(&worker->mutex);

    pthread_join(worker->thread, NULL);

    pthread_mutex_destroy(&worker->mutex);
    pthread_cond_destroy(&worker->cond);
    linkedlist_delete(worker->sth_todo, NULL);
    free(worker);
    return;
}



static void work(_lazyworker_t* worker)
{
	_unit* unit = NULL;
    static struct timespec time_wait;

    for(;worker->threadStatus == eThreadStatus_RUNNING;)
    {
        pthread_mutex_lock(&worker->mutex);
        for(;;)
        {
            int que_size = linkedlist_size(worker->sth_todo);
            if(que_size > 0)
            {
                break;
            }
            pthread_cond_wait(&worker->cond, &worker->mutex);
            if(worker->threadStatus == eThreadStatus_CLOSED)
            {
                pthread_mutex_unlock(&worker->mutex);
                pthread_exit((void*) NULL);
            }
        }

        fprintf(stderr, "who wakes me up?");
        struct timeval current;
        gettimeofday(&current, NULL);
        unit = (_unit*)linkedlist_get(worker->sth_todo, 0);
        if(current.tv_sec - unit->t.tv_sec < 1)
        {
            // Be lazy
            time_wait.tv_sec =  unit->t.tv_sec + worker->time_wait.tv_sec;
            time_wait.tv_nsec = unit->t.tv_usec * 1000 + worker->time_wait.tv_nsec;
            fprintf(stderr, "I am too lazy to work, I'll be back in 1 minute. OK?");
            pthread_cond_timedwait(&worker->cond, &worker->mutex, &time_wait);
        }
        else
        {
            unit = (_unit*)linkedlist_remove_at(worker->sth_todo, 0);
            free(unit);
            if(worker->fp != NULL)
            {
            	fprintf(stderr, "OK, now I am ready to work, what's on your mind?\n");
            	worker->fp(worker->args);
            }
        }
        pthread_mutex_unlock(&worker->mutex);
    } // for
}
