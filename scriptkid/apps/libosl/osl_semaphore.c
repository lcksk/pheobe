/*
 * kaonsys_semaphore.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */


#include <pthread.h>
#include "osl.h"
#include "osldbg.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>

typedef struct
{
	u_int32_t 						count;
	osl_mutex						countLock;
	u_int32_t							countMax;
	pthread_cond_t			cond;
} osl_semaphore_t;


osl_semaphore oslCreateSemaphore(void)
{
	return oslCreateCountSemaphore(1);
}

void oslInitCountSemaphore(osl_semaphore semaphore, u_int32_t value)
{
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	oslMutexLock(self->countLock);
	self->count = value;
	self->countMax = value;
	oslMutexUnlock(self->countLock);
}

osl_semaphore oslCreateCountSemaphore(u_int32_t value)
{
	osl_semaphore_t* obj = (osl_semaphore_t*) malloc(sizeof(osl_semaphore_t));
	if(obj == NULL) 	{
		ERR("Fatal error");
		return NULL;
	}
	obj->countLock = KnOSCreateMutex();
	oslInitCountSemaphore(obj, value);
	pthread_cond_init(&obj->cond, NULL);
	return (osl_semaphore) obj;
}

void oslDeleteSemaphore(osl_semaphore semaphore)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	oslDeleteMutex(self->countLock);
	pthread_cond_destroy(&self->cond);
	free(self);
}


bool oslSemaphoreWaitTimeout (osl_semaphore semaphore, u_int16_t timeout)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	oslMutexLock(self->countLock);
	self->count--;
	if(self->count == 0) {
		if(timeout == OSL_INFINITE) {
			pthread_cond_wait(&self->cond, (pthread_mutex_t*)self->countLock);
		}
		else {
			struct timeval now;
			struct timespec ts;
			gettimeofday(&now, NULL);
			ts.tv_sec = now.tv_sec;
			ts.tv_nsec = (now.tv_usec +  timeout) * 1000;
			pthread_cond_timedwait(&self->cond, (pthread_mutex_t*)self->countLock, &ts);
			oslMutexUnlock(self->countLock);
			return false;
		}
	}
	if(self->count < 0)
		self->count = 0;

	oslMutexUnlock(self->countLock);
	return true;
}


void oslSemaphoreWait(osl_semaphore semaphore)
{
	oslSemaphoreWaitTimeout(semaphore, OSL_INFINITE);
}

void oslSemaphoreSignal(osl_semaphore semaphore)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;

	oslMutexLock(self->countLock);
	self->count++;
	if(self->countMax < self->count)
		self->count = self->countMax;

	oslMutexUnlock(self->countLock);
	pthread_cond_signal(&self->cond);
}

