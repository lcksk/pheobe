/*
 * kaonsys.h
 *
 *  Created on: Sep 10, 2010
 *      Author: buttonfly
 */

#ifndef OSL_H_
#define OSL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <sys/types.h>

typedef enum {
	OSL_SUCCESS = 0,
	OSL_ERROR_OUT_OF_MEMORY,
	OSL_ERROR_INVALID_PARAM,
	OSL_NOT_SUPPORTED,
	OSL_NOT_AVAILABLE,
	OSL_ERROR_BUSY,
	OSL_EXTERNAL_ERROR,
	OSL_INVALID_STATES,
	OSL_TIMEOUT,
	OSL_RESET_REQUIRED,
	OSL_FAILTURE,
	OSL_MUCH_TIME_DIFFERENCE_ERROR,
	OSL_SMARTCARD_ERROR,
	OSL_SMARTCARD_ERROR_RX_PARITY,
	OSL_SMARTCARD_ERROR_TX_PARITY,
	OSL_SMARTCARD_ERROR_RX_TIMEOUT,
	OSL_SMARTCARD_ERROR_TX_TIMEOUR,
	OSL_SMARTCARD_ERROR_HW_FAILURE,
	OSL_SMARTCARD_RESET_TERMINAL, //requires a call to bsmartcard_reset()
} OSL_Error;


typedef void* osl;
typedef void* osl_mutex;
typedef void* osl_thread;
typedef void* osl_semaphore;
typedef void* osl_queue;

#define OSL_INFINITE		0xFFFF
#define OSL_TIMEOUT		20

#define _out_

//unsigned int oslGetClockMilliseconds(void);
u_int32_t osl_get_clock_ms (void);
/**
 *
 * @function oslCreateThread
 * @param	function		pointer function
 * @param 	param			argument to be aligned when the [fucntion] is called
 * @param	stack			the specific stack size. if you set 0, it would be set as default stack size
 * @param	name			@deprecated
 * @return						handle
 */
//osl_thread oslCreateThread(void (*__function)(void*), void* param, u_int32_t stack, u_int8_t priority, int8_t* name);
osl_thread osl_thread_create (void (*__function)(void*), void* param, u_int32_t stack, u_int8_t priority, int8_t* name);

/**
 * @function oslThreadDelay
 * @param	timeout			millisecond
 */
//void oslThreadDelay (u_int32_t timeout);
void osl_delay (u_int32_t timeout);

/**
 * @function	oslThreadSleep
 * @param	timeout			second
 */
//void oslThreadSleep(u_int32_t timeout);
void osl_sleep (u_int32_t timeout);

/**
 * @function	KnOSGetCurrentTask
 * @return						current thread
 */
//osl_thread oslGetCurrentTask(void);
osl_thread osl_thread_current(void);

/**
 * @function	KnOSCreateSemaphore
 * @return	semaphore handle
 */
//osl_semaphore oslCreateSemaphore(void);
osl_semaphore osl_semaphore_create(void);

/**
 * @function KnOSCreateCountSemaphore
 * @param 	value
 */
//osl_semaphore oslCreateCountSemaphore(u_int32_t value);
osl_semaphore osl_semaphore_create_count(u_int32_t count);

/**
 * @function KnOSInitCountSemaphore
 * @param	semaphore	semaphore handle
 * @param	value
 * @deprecated		It is used internally
 */
//void oslInitCountSemaphore(osl_semaphore semaphore, u_int32_t value);
void osl_semaphore_init(osl_semaphore semaphore, u_int32_t count);

/**
 * @function	KnOSDeleteSemaphore
 * @param	semaphore
 * @description
 */
//void oslDeleteSemaphore(osl_semaphore semaphore);
void osl_semaphore_close(osl_semaphore semaphore);

/**
 * @function	KnOSSemaphoreWait
 * @param	semaphore
 * @description
 * The running status if the current thread is changed to the status of NOT_RUNNABLE
 */
//void oslSemaphoreWait(osl_semaphore semaphore);
void osl_semaphore_wait(osl_semaphore semaphore);
/**
 * @function KnOSSemaphoreSignal
 * @param	semaphore
 * @description
 * wake up
 */
//void oslSemaphoreSignal(osl_semaphore semaphore);
void osl_semaphore_signal(osl_semaphore semaphore);

/***
 * @function	KnOSSemaphoreWaitTimeout
 * @param	semaphore
 * @param	timeout				milli
 * @return
 */
//bool oslSemaphoreWaitTimeout (osl_semaphore semaphore, u_int16_t timeout);
bool osl_semaphore_timed_wait(osl_semaphore semaphore, u_int16_t timeout);

/**
 * @function	KnOSCreateMutex
 * @return	mutex handle
 */
//osl_mutex oslCreateMutex (void);
osl_mutex osl_mutex_create(void);

/**
 * @function	KnOSMutexLock
 * @param	handle
 */
//void oslMutexLock (osl_mutex mutex);
void osl_mutex_lock(osl_mutex mutex);

/**
 * @function	KnOSMutexUnlock
 * @param	handle
 */
void osl_mutex_unlock(osl_mutex mutex);

/**
 * @function	KnOSMutexUnlock
 * @param	handle
 */
void osl_mutex_close (osl_mutex mutex);


u_int32_t osl_get_gateway_addr(void);

u_int32_t osl_get_hwaddr(u_int8_t *addr);

typedef void* osl_random;

osl_random osl_random_open();

u_int32_t osl_random_read(osl_random random, u_int8_t* buf, u_int32_t len);

void osl_random_close(osl_random random);

typedef struct {
	u_int32_t					event;
} osl_event;



#if 0
#include <stdlib.h>
#define KASSERT
#else
#include <stdlib.h>
#define KASSERT(x)	do{ if(!(x)){ERR("Assertion failed in line"); abort(); }; }while(0)
#endif




#ifdef __cplusplus
}
#endif


#endif /* OSL_H_ */
