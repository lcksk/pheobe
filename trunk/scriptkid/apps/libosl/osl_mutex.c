/*
 * kaonsys_mutex.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "osl.h"
#include "osldbg.h"
#include <string.h>

osl_mutex KnOSCreateMutex (void)
{
	pthread_mutex_t* lck;
	pthread_mutexattr_t mutexAttr;;
	lck = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	KASSERT(lck);
//	pthread_mutexattr_init(&mutexAttr);

//	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
//	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_FAST_NP);
//	pthread_mutex_init(mutex_p, &mutexAttr);
	pthread_mutex_init(lck, NULL);
	return (osl_mutex) lck;
}

void oslMutexLock (osl_mutex mutex)
{
	KASSERT(mutex);
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void oslMutexUnlock (osl_mutex mutex)
{
	KASSERT(mutex);
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

void oslDeleteMutex (osl_mutex mutex)
{
	KASSERT(mutex);
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
	free(mutex);
}

