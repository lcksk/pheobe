/*
 * kaonsys_task.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "osl.h"
#include "osldbg.h"

osl_thread osl_thread_create(void (*__function)(void*), void* param, u_int32_t stack, u_int8_t priority, int8_t* name)
{
	S_;
    pthread_attr_t        attribute;
    pthread_attr_init(&attribute);
    pthread_attr_setscope(&attribute, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

    if(priority > 0) {
    	int policy;
    	struct sched_param sched;
    	int priority_max, priority_min;
    	pthread_getschedparam(pthread_self(), &policy, &sched);
    	priority_max = sched_get_priority_max(policy);
    	priority_min = sched_get_priority_min(policy);
    	M_("priority : %d ~ %d", priority_min, priority_max);
    	sched.sched_priority = priority;
    	pthread_attr_setschedparam(&attribute,&sched);
    	pthread_getschedparam(pthread_self(), &policy, &sched);
    	M_("sched_priority: %d", sched.sched_priority);
    }

    if(stack > 0) {
    	size_t stacksize;
    	pthread_attr_getstacksize(&attribute, &stacksize);
    	pthread_attr_setstacksize(&attribute, stack);
    }

    pthread_t thread;
    pthread_create(&thread, &attribute, (void* (*)(void*)) __function, (void*)param);

    pthread_attr_destroy(&attribute);

    return (void*)thread;
}

void osl_delay (u_int32_t timeout) {
	usleep(timeout);
}

void osl_sleep(u_int32_t timeout)
{
	sleep(timeout);
}

osl_thread osl_thread_current(void)
{
	return (void*)pthread_self();
}

