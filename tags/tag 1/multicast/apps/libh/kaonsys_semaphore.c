/*
 * kaonsys_semaphore.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "kaonsys.h"
#include "kaonutil.h"
#include <time.h>
#include <sys/time.h>


typedef struct
{
	U32BIT 					count;
	KnMutex_t				countLock;
	U32BIT						countMax;
	pthread_cond_t			cond;
} _knSemaphore_t;


KnSemaphore_t KnOSCreateSemaphore(void)
{
	return KnOSCreateCountSemaphore(1);
}

void KnOSInitCountSemaphore(KnSemaphore_t semaphore,U32BIT value)
{
	_knSemaphore_t* sema_p = (_knSemaphore_t*) semaphore;
	KnOSMutexLock(sema_p->countLock);
	sema_p->count = value;
	sema_p->countMax = value;
	KnOSMutexUnlock(sema_p->countLock);
}

KnSemaphore_t KnOSCreateCountSemaphore(U32BIT value)
{
	_knSemaphore_t* sema_p = (_knSemaphore_t*) KnMalloc(sizeof(_knSemaphore_t));
	if(sema_p == NULL)
	{
		ERR("Fatal error");
		return NULL;
	}
	sema_p->countLock = KnOSCreateMutex();
	KnOSInitCountSemaphore(sema_p, value);
	pthread_cond_init(&sema_p->cond, NULL);
	return (KnSemaphore_t)sema_p;
}

void KnOSDeleteSemaphore(KnSemaphore_t semaphore)
{
	if(semaphore == NULL)
	{
		ERR("Fatal error");
		return;
	}
	_knSemaphore_t* sema_p = (_knSemaphore_t*) semaphore;
	KnOSDeleteMutex(sema_p->countLock);
	pthread_cond_destroy(&sema_p->cond);
	KnFree(sema_p);
}


BOOLEAN KnOSSemaphoreWaitTimeout (KnSemaphore_t semaphore, U16BIT timeout)
{
	KASSERT(semaphore);
	_knSemaphore_t* sema_p = (_knSemaphore_t*) semaphore;
	KnOSMutexLock(sema_p->countLock);
	sema_p->count--;
	if(sema_p->count == 0)
	{
		if(timeout == KN_INFINITE)
		{
			pthread_cond_wait(&sema_p->cond, (pthread_mutex_t*)sema_p->countLock);
		}
		else
		{
			struct timeval now;
			struct timespec ts;
			gettimeofday(&now, NULL);
			ts.tv_sec = now.tv_sec;
			ts.tv_nsec = (now.tv_usec +  timeout) * 1000;
			pthread_cond_timedwait(&sema_p->cond, (pthread_mutex_t*)sema_p->countLock, &ts);
			KnOSMutexUnlock(sema_p->countLock);
			return FALSE;
		}
	}
	if(sema_p->count < 0)
	{
		sema_p->count = 0;
	}
	KnOSMutexUnlock(sema_p->countLock);
	return TRUE;
}


void KnOSSemaphoreWait(KnSemaphore_t semaphore)
{
	KnOSSemaphoreWaitTimeout(semaphore, KN_INFINITE);
}

void KnOSSemaphoreSignal(KnSemaphore_t semaphore)
{
	KASSERT(semaphore);
	_knSemaphore_t* sema_p = (_knSemaphore_t*) semaphore;
	KnOSMutexLock(sema_p->countLock);
	sema_p->count++;
	if(sema_p->countMax < sema_p->count)
	{
		sema_p->count = sema_p->countMax;
	}
	KnOSMutexUnlock(sema_p->countLock);
	pthread_cond_signal(&sema_p->cond);
}

