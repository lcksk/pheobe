/*
 * lazyworker.h
 *
 *  Created on: Jun 2, 2011
 *      Author: buttonfly
 */

#ifndef LAZYWORKER_H_
#define LAZYWORKER_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* lazyworker;

#include <sys/time.h>

typedef void (*lazyworker_workCallback)(void* context);

lazyworker lazyworker_new(struct timespec* time_wait, lazyworker_workCallback, void* args);

void lazyworker_invokeLater(lazyworker);

void lazyworker_delete(lazyworker);


#ifdef __cplusplus
}
#endif

#endif /* LAZYWORKER_H_ */
