/*
 * kaonsys_task.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "kaonsys.h"
#include "kaonutil.h"

KnThread_t KnOSCreateTask(void (*function)(void*), void* param, U32BIT stack, U8BIT priority, U8BIT* name)
{
	S_;
    pthread_attr_t        attribute;
    pthread_attr_init(&attribute);
    pthread_attr_setscope(&attribute, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);
    struct sched_param sched;
    pthread_attr_getschedparam(&attribute,&sched);
    if(priority > 0)
    {
    	sched.sched_priority = priority;
    }
    pthread_attr_setschedparam(&attribute,&sched);

    size_t stacksize;
    pthread_attr_getstacksize(&attribute, &stacksize);
    if(stack > 0)
    {
    	pthread_attr_setstacksize(&attribute, stack);
    }

    pthread_t thread;
    pthread_create(&thread, &attribute, (void* (*)(void*)) function, (void*)param);

    pthread_attr_destroy(&attribute);

    return (void*)thread;
}

void KnOSTaskDelay (U32BIT timeout)
{
	usleep(timeout);
}

void KnOSTaskSleep(U32BIT timeout)
{
	sleep(timeout);
}

KnThread_t KnOSGetCurrentTask(void)
{
	return (void*)pthread_self();
}

