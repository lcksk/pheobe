/*
 * kaonsys.mq.c
 *
 *  Created on: Sep 17, 2010
 *      Author: buttonfly
 */

#include "kaonsys.h"
#include "kaonutil.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>

// remove log
#define S_
#define Q_
#define M_
#define ERR
#define DOT_
#define NOT_IMPL



#define _SW_Q_

#ifdef _SW_Q_
#include "linkedlist.h"
#else
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#endif


static long long knGetTimeDiff(struct timeval* val1, struct timeval* val2);

typedef struct
{
#ifdef _SW_Q_
	linkedlist				queue;
#else
	int						fd;
#endif
	KnMutex_t			lock;
	KnSemaphore_t	event;
} knQueue_t;

typedef struct
{
	int					msgId;
	KnMutex_t		semaLock;
} knSystem_t;

#define	CI_MSG_KEY			0x2000


static knSystem_t* knSysConstruct(void);
static knSystem_t* knSysGetInstance(void);

static knQueue_t* knCreateQueue();
static void knDestroyQueue(knQueue_t* que);
static void knReleaseEventNode(void* data_p);


KnQueue_t KnOSCreateQueue(U16BIT msgSize, U16BIT msgMax)
{
	S_;
	knSystem_t* sys_p = knSysGetInstance();
	sys_p->msgId++;

	knQueue_t* que = knCreateQueue();
	return que;
}



BOOLEAN KnOSDestroyQueue(KnQueue_t queue)
{
	knDestroyQueue((knQueue_t*)queue);
	return TRUE;
}


BOOLEAN KnOSReadQueue(KnQueue_t queue, void* msg, U16BIT msg_size, U16BIT timeout)
{
	S_;
#ifdef _SW_Q_
	BOOLEAN flag = FALSE;
	knQueue_t* que = (knQueue_t*) queue;

	for(;;)
	{
		int size;
		KnOSMutexLock(que->lock);
		size = linkedlist_size(que->queue);
		KnOSMutexUnlock(que->lock);
		if(size > 0)
		{
			M_("size : %d", size);
			break;
		}
		else
		{
			if(KN_INFINITE != timeout)
			{
				if(flag == TRUE)
				{
					return FALSE;
				}
			}
			KnOSSemaphoreWaitTimeout(que->event, timeout);
			flag = TRUE;
		}
	}
	KnOSMutexLock(que->lock);
	void* data_p = linkedlist_remove_at(que->queue, 0);
//	KASSERT(data_p);
	if(data_p == NULL)
	{
		KnOSMutexUnlock(que->lock);
		return FALSE;
	}
	KnOSMutexUnlock(que->lock);
	KnMemcpy(msg, data_p, msg_size);
	return TRUE;


#else
	int flag;
	if(timeout == KN_INFINITE)
	{
		Q_;
		flag = 0;
	}
	else
	{
		Q_;
		flag = IPC_NOWAIT;
	}
	knQueue_t* que = (knQueue_t*) queue;
	Q_;
	KnOSMutexLock(que->lock);
	Q_;
	int ret= msgrcv(que->fd,(void *)msg, msg_size, 0, flag | MSG_NOERROR);
	Q_;
	KnOSMutexUnlock(que->lock);
	if(ret != -1)
	{
		Q_;
		return TRUE;
	}
	Q_;
	return FALSE;
#endif
}


BOOLEAN KnOSWriteQueue(KnQueue_t queue, void* msg, U16BIT msg_size, U16BIT timeout)
{
	S_;
#ifdef _SW_Q_
	knQueue_t* que = (knQueue_t*) queue;
	void* data_p = KnMalloc(msg_size);
	KnMemcpy(data_p, msg, msg_size);
	KnOSMutexLock(que->lock);
	linkedlist_add(que->queue, data_p);
	KnOSMutexUnlock(que->lock);
	KnOSSemaphoreSignal(que->event);
	return TRUE;
#else
	knQueue_t* que = (knQueue_t*) queue;
	Q_;
	KnOSMutexLock(que->lock);
	Q_;
	int ret = msgsnd(que->fd, msg, msg_size, 0 | MSG_NOERROR);
	Q_;
	if(ret == -1)
	{
		Q_;
		KnOSMutexUnlock(que->lock);
		return FALSE;
	}
	KnOSMutexUnlock(que->lock);
	KnOSSemaphoreSignal(que->event);
	return TRUE;
#endif
}

static void finalize(void* data)
{
	if(data != NULL)
	{
		KnFree(data);
		data = NULL;
	}
}

BOOLEAN KnOSRemoveAllQueue(KnQueue_t queue)
{
	knQueue_t* que = (knQueue_t*) queue;
	KnOSMutexLock(que->lock);
	linkedlist_clear(que->queue, finalize);
	KnOSMutexUnlock(que->lock);
	return TRUE;
}

knSystem_t* knSysConstruct(void)
{
	knSystem_t* sys_p = (knSystem_t*) KnMalloc(sizeof(knSystem_t));
	memset(sys_p, 0, sizeof(knSystem_t));
	sys_p->msgId = CI_MSG_KEY;
	sys_p->semaLock = KnOSCreateMutex();
	return sys_p;
}


knSystem_t* knSysGetInstance(void)
{
	static knSystem_t* sys_p = NULL;
	if(sys_p == NULL)
	{
		sys_p = knSysConstruct();
	}
	return sys_p;
}


//#ifdef _SW_Q_
knQueue_t* knCreateQueue()
{
	knQueue_t* que = (knQueue_t*)malloc(sizeof(knQueue_t));
#ifdef _SW_Q_
	que->queue = linkedlist_new();
#else
	que->fd = msgget(knSysGetInstance()->msgId, IPC_CREAT | 0660);
	if(que->fd == -1)
	{
		Q_;
		return NULL;
	}
#endif
	que->lock = KnOSCreateMutex();
	que->event = KnOSCreateSemaphore();
	return que;
}


void knDestroyQueue(knQueue_t* que)
{
	if(que == NULL)
	{
		return;
	}

#ifdef _SW_Q_
	linkedlist_delete(que->queue, knReleaseEventNode);
#else
	msgctl(que->fd, IPC_RMID, 0);
#endif

	KnOSDeleteMutex(que->lock);
	KnOSDeleteSemaphore(que->event);
	KnFree(que);
}


long long knGetTimeDiff(struct timeval* val1, struct timeval* val2)
{
	unsigned long long sec = val2->tv_sec - val1->tv_sec;
	unsigned long long usec = val2->tv_usec - val1->tv_usec;
	long long diff = sec * 1000000LL + usec;
	return diff;
}

void knReleaseEventNode(void* data_p)
{
	KnFree(data_p);
}



//#endif

