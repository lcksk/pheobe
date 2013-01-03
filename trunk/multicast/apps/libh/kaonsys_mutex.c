/*
 * kaonsys_mutex.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "kaonsys.h"
#include "kaonutil.h"
//#include "casdbg.h"

KnMutex_t KnOSCreateMutex (void)
{
	pthread_mutex_t* mutex_p;
	pthread_mutexattr_t mutexAttr;;
	mutex_p = (pthread_mutex_t*) KnMalloc(sizeof(pthread_mutex_t));
	KASSERT(mutex_p);
//	pthread_mutexattr_init(&mutexAttr);

//	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
//	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_FAST_NP);
//	pthread_mutex_init(mutex_p, &mutexAttr);
	pthread_mutex_init(mutex_p, NULL);
	return (KnMutex_t) mutex_p;
}

void KnOSMutexLock (KnMutex_t mutex)
{
	KASSERT(mutex);
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void KnOSMutexUnlock (KnMutex_t mutex)
{
	KASSERT(mutex);
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

void KnOSDeleteMutex (KnMutex_t mutex)
{
	KASSERT(mutex);
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
}

