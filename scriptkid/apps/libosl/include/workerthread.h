#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* workerthread;


/**
 * create a worker thread pool as many as the specified number
 * @param numofthread the number of thread(s) to create
 * @return this thread pool
 */
extern workerthread workerthread_new(int numofthread);

/**
 * close this worker thread pool
 * @param workerthread this worker thread pool
 */
extern void workerthread_delete(workerthread workerthread);

/**
 * queue with the specified task in this worker thread pool.
 * before you call this function, you need to implement
 * a function which will be used as routine and complete_callback in the unittask_create
 * @param workerthread the worker thread
 * @param task the unittask to be queued
 */
extern void workerthread_put_task(workerthread workerthread, void (*routine)(void*), void* arg, void(*complete_callback)(void*));

extern void workerthread_remove_all_task(workerthread _workerthread);

#ifdef __cplusplus
}
#endif

#endif /*WORKERTHREAD_H_*/
