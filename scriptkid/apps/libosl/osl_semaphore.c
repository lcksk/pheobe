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
	osl_mutex						lck;
	u_int32_t							countMax;
	pthread_cond_t			cond;
} osl_semaphore_t;


osl_semaphore osl_semaphore_create(void)
{
	return osl_semaphore_create_count(1);
}

void osl_semaphore_init(osl_semaphore semaphore, u_int32_t value)
{
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	osl_mutex_lock(self->lck);
	self->count = value;
	self->countMax = value;
	osl_mutex_unlock(self->lck);
}

osl_semaphore osl_semaphore_create_count(u_int32_t value)
{
	osl_semaphore_t* obj = (osl_semaphore_t*) malloc(sizeof(osl_semaphore_t));
	if(obj == NULL) 	{
		ERR("Fatal error");
		return NULL;
	}
	obj->lck = osl_mutex_create();
	osl_semaphore_init(obj, value);
	pthread_cond_init(&obj->cond, NULL);
	return (osl_semaphore) obj;
}

void osl_semaphore_close(osl_semaphore semaphore)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	osl_mutex_close(self->lck);
	pthread_cond_destroy(&self->cond);
	free(self);
}


bool osl_semaphore_timed_wait (osl_semaphore semaphore, u_int16_t timeout)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;
	osl_mutex_lock(self->lck);
	self->count--;
	if(self->count == 0) {
		if(timeout == OSL_INFINITE) {
			pthread_cond_wait(&self->cond, (pthread_mutex_t*)self->lck);
		}
		else {
			struct timeval now;
			struct timespec ts;
			gettimeofday(&now, NULL);
			ts.tv_sec = now.tv_sec;
			ts.tv_nsec = (now.tv_usec +  timeout) * 1000;
			pthread_cond_timedwait(&self->cond, (pthread_mutex_t*)self->lck, &ts);
			osl_mutex_unlock(self->lck);
			return false;
		}
	}
	if(self->count < 0)
		self->count = 0;

	osl_mutex_unlock(self->lck);
	return true;
}


void osl_semaphore_wait(osl_semaphore semaphore)
{
	osl_semaphore_timed_wait(semaphore, OSL_INFINITE);
}

void osl_semaphore_signal(osl_semaphore semaphore)
{
	KASSERT(semaphore);
	osl_semaphore_t* self = (osl_semaphore_t*) semaphore;

	osl_mutex_lock(self->lck);
	self->count++;
	if(self->countMax < self->count)
		self->count = self->countMax;

	osl_mutex_unlock(self->lck);
	pthread_cond_signal(&self->cond);
}

