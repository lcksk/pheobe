/*
 * kaonsys_task.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "osl.h"
#include "osldbg.h"

osl_thread oslCreateThread(void (*__function)(void*), void* param, u_int32_t stack, u_int8_t priority, int8_t* name)
{
	S_;
    pthread_attr_t        attribute;
    pthread_attr_init(&attribute);
    pthread_attr_setscope(&attribute, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);
    struct sched_param sched;
    pthread_attr_getschedparam(&attribute,&sched);
    if(priority > 0) {
    	sched.sched_priority = priority;
    }
    pthread_attr_setschedparam(&attribute,&sched);

    size_t stacksize;
    pthread_attr_getstacksize(&attribute, &stacksize);
    if(stack > 0) {
    	pthread_attr_setstacksize(&attribute, stack);
    }

    pthread_t thread;
    pthread_create(&thread, &attribute, (void* (*)(void*)) __function, (void*)param);

    pthread_attr_destroy(&attribute);

    return (void*)thread;
}

void oslThreadDelay (u_int32_t timeout) {
	usleep(timeout);
}

void oslThreadSleep(u_int32_t timeout)
{
	sleep(timeout);
}

osl_thread oslGetCurrentTask(void)
{
	return (void*)pthread_self();
}

